#include "Server.hpp"
#include "Game.hpp"
#include "network/Network.hpp"
#include "network/NetHelper.hpp"
#include "VersionInfo.hpp"
#include "CaveGenerator.hpp"
#include <iterator>
#include <algorithm>
#include <thread>
#include <sstream>

using std::cout;
using std::endl;

using namespace Diggler::Net;

namespace Diggler {

Player* Server::getPlayerByPeer(const Peer &peer) {
	try {
		return &G.players.getByPeer(peer);
	} catch (const std::out_of_range &e) {
		return nullptr;
	}
}
Player* Server::getPlayerById(uint32 id) {
	try {
		return &G.players.getById(id);
	} catch (const std::out_of_range &e) {
		return nullptr;
	}
}
Player* Server::getPlayerByName(const std::string &name) {
	try {
		return &G.players.getByName(name);
	} catch (const std::out_of_range &e) {
		return nullptr;
	}
}

void Server::handlePlayerJoin(InMessage &msg, Peer &peer) {
	std::string name = msg.readString();
	getOutputStream() << peer.getHost() << " is joining as " << name << std::endl;
	
	// TODO: ban list
	Player *possible = getPlayerByName(name);
	if (possible != nullptr) {
		// TODO: use kick() method
		OutMessage kick(MessageType::PlayerQuit, QuitReason::UsernameAlreadyUsed);
		kick.writeString("You are \faalready\f0 playing on this server");
		H.send(peer, kick, Tfer::Rel);
		getOutputStream() << peer.getHost() << " tried to connect as " << name << ": name is taken" << endl;
		return;
	}

	Player &plr = G.players.add();
	plr.name = name;
	plr.id = FastRand();
	plr.P = peer;
	plr.W = G.U->getWorldEx(0);

	// Confirm successful join
	OutMessage join(MessageType::PlayerJoin);
	join.writeU32(plr.id);
	join.writeI16(plr.W->id);
	H.send(peer, join, Tfer::Rel);
	
	// Send the player list
	for (Player &p : G.players) {
		if (p.id == plr.id)
			continue; // ok, he knows he's here
		OutMessage playerMsg(MessageType::PlayerJoin);
		playerMsg.writeU32(p.id);
		playerMsg.writeString(p.name);
		H.send(peer, playerMsg, Tfer::Rel);
	}
	
	// Broadcast player's join
	OutMessage broadcast(MessageType::PlayerJoin);
	broadcast.writeU32(plr.id);
	broadcast.writeString(plr.name);
	for (Player &p : G.players) {
		if (p.id == plr.id)
			continue; // don't send broadcast to the player
		H.send(p.P, broadcast, Tfer::Rel);
	}
	getOutputStream() << plr.name << " joined from " << peer.getHost() << endl;
	schedSendChunk(G.U->getWorld(0)->getChunkEx(0, 0, 0), plr);
}

void Server::handlePlayerQuit(Peer &peer, QuitReason reason) {
	Player *plrPtr = getPlayerByPeer(peer);
	if (plrPtr) {
		Player &plr = *plrPtr;
		// Broadcast disconnection
		OutMessage broadcast(MessageType::PlayerQuit, reason);
		broadcast.writeU32(plr.id);
		for (Player &p : G.players) {
			if (p.id == plr.id)
				continue; // dont send broadcast to the player
			H.send(p.P, broadcast, Tfer::Rel);
		}
		getOutputStream() << plr.name << " disconnected" << endl;
		G.players.remove(plr);
	} else {
		getOutputStream() << peer.getHost() << " disconnected" << endl;
	}
}

void Server::handleDisconnect(Peer &peer) {
	handlePlayerQuit(peer, QuitReason::Timeout);
}

void Server::handleChat(InMessage &msg, Player *plr) {
	std::string chatMsg = msg.readString();
	std::string senderName = plr ? plr->name : "<console>";
	if (chatMsg.length() > 1 && chatMsg[0] == '/') {
		size_t pos = chatMsg.find_first_of(' ');
		if (pos == std::string::npos) {
			handleCommand(plr, chatMsg.substr(1), std::vector<std::string>());
		} else {
			handleCommand(plr, chatMsg.substr(1, pos-1), std::vector<std::string>());
		}
	} else {
		// TODO: implement UTF8
		getOutputStream() << senderName << ": " << chatMsg << endl;
		std::ostringstream contentFormatter;
		contentFormatter << senderName << "> " << chatMsg;
		std::string content = contentFormatter.str();
		OutMessage newMsg(MessageType::Chat);
		newMsg.writeString(content);
		NetHelper::Broadcast(G, newMsg, Tfer::Rel);
	}
}

void Server::handleEvent(InMessage &msg, Peer &peer) {
	Player &plr = G.players.getByPeer(peer);
	switch (msg.getSubtype()) {
	case Net::EventType::PlayerJumpOnPad: {
		OutMessage out;
		NetHelper::MakeEvent(out, (Net::EventType)msg.getSubtype(), plr);
		NetHelper::Broadcast(G, out);
	} break;

	default:
		break;
	}
}

void Server::handleCommand(Player *plr, const std::string &command, const std::vector<std::string> &args) {
	getDebugStream() << "Command \"" << command << '"' << std::endl;
}

void Server::handlePlayerUpdate(InMessage &msg, Player &plr) {
	switch (msg.getSubtype()) {
	case PlayerUpdateType::Move: {
		// Broadcast movement
		OutMessage bcast(MessageType::PlayerUpdate, PlayerUpdateType::Move);
		bcast.writeU32(plr.id);
		glm::vec3 pos = msg.readVec3(),
				vel = msg.readVec3(),
				acc = msg.readVec3();
		plr.setPosVel(pos, vel, acc);
		plr.angle = msg.readFloat();
		bcast.writeVec3(pos);
		bcast.writeVec3(vel);
		bcast.writeVec3(acc);
		bcast.writeFloat(plr.angle);
		for (Player &p : G.players) {
			if (p.id == plr.id)
				continue; // dont send broadcast to the player
			// TODO: confirm position to player
			H.send(p.P, bcast, Tfer::Unrel);
		}
	} break;
	case PlayerUpdateType::Die:
		handlePlayerDeath(msg, plr);
		break;
	default:
		break;
	}
}

void Server::schedSendChunk(ChunkRef C, Player &P) {
	glm::ivec3 pos = C->getWorldChunkPos();
	getDebugStream() << "Pend Chunk[" << pos.x << ',' << pos.y << ' ' << pos.z <<
		"] send to " << P.name << std::endl;
	P.pendingChunks.emplace_back(C);
}

void Server::sendChunk(Chunk &C, Player &P) {
	OutMessage msg(MessageType::ChunkTransfer);
	msg.writeU8(1); // Nbr of sent chunks
	msg.writeI16(C.getWorld()->id);
	glm::ivec3 pos = C.getWorldChunkPos();
	msg.writeI16(pos.x);
	msg.writeI16(pos.y);
	msg.writeI16(pos.z);
	C.send(msg);
	H.send(P.P, msg, Tfer::Rel, Channels::MapUpdate);
}

void Server::handlePlayerChunkRequest(InMessage &msg, Player &plr) {
	// TODO: distance & tool check, i.e. legitimate update
	int cx = msg.readI16(), cy = msg.readI16(), cz = msg.readI16();
	schedSendChunk(plr.W->getChunkEx(cx, cy, cz), plr);
}

void Server::handlePlayerMapUpdate(InMessage &msg, Player &plr) {
	// TODO: distance & tool check, i.e. legitimate update
	WorldId wid = msg.readI16();
	int x = msg.readI32(), y = msg.readI32(), z = msg.readI32();
	BlockId id = msg.readU16();
	BlockData data = msg.readU16(); 
	ChunkRef c = G.U->getWorld(wid)->getChunkAtCoords(x, y, z);
	if (c) {
		c->setBlock(x, y, z, id, data);
		// TODO: block datrees
		// FIXME: v This might interfere with the ticker
		if (!c->CH.empty()) {
			OutMessage msg(MessageType::ChunkUpdate, c->CH.count());
			c->CH.flush(msg);
			NetHelper::Broadcast(G, msg, Tfer::Rel, Channels::MapUpdate);
		}
	}
}

void Server::handlePlayerDeath(InMessage &msg, Player &plr) {
	uint8 drb = msg.readU8();
	Player::DeathReason dr = (Player::DeathReason)drb;
	plr.setDead(false, dr);
	OutMessage out(MessageType::PlayerUpdate, PlayerUpdateType::Die);
	out.writeU32(plr.id);
	out.writeU8(drb);
	for (Player &p : G.players) {
		if (p.id != plr.id)
			H.send(p.P, out, Tfer::Rel, Channels::Life);
	}
	
	// Respawn player later
	Game *G = &this->G; uint32 id = plr.id;
	std::thread respawn([G, id] {
		std::this_thread::sleep_for(std::chrono::seconds(2));
		Player *plr = G->S->getPlayerById(id);
		if (plr) {
			plr->setDead(false);
			OutMessage out(MessageType::PlayerUpdate, PlayerUpdateType::Respawn);
			out.writeU32(id);
			NetHelper::Broadcast(G, out, Tfer::Rel, Channels::Life);
		}
	});
	respawn.detach();
}

Server::Server(Game &G, uint16 port) : G(G) {
	G.init();

	getOutputStream() << "Diggler v" << VersionString << " Server, port " << port << ", "
		<< std::thread::hardware_concurrency() << " HW threads supported" << endl;

	if (port >= 49152) {
		getErrorStream() << "Warning: port is within the Ephemeral Port Range as defined by IANA!" <<
			std::endl << "  Nothing wrong with that, but for compatibility's sake please avoid this range." <<
			std::endl;
	}

	try {
		H.create(port);
	} catch (Net::Exception &e) {
		getErrorStream() << "Couldn't open port " << port << " for listening" << endl <<
			"Make sure no other server instance is running" << endl;
		throw "Server init failed";
	}

	startInternals();
	start();
}

void Server::startInternals() {
	LS = luaL_newstate();
	luaL_openlibs(LS);
}

// FIXME ugly ugly hack to keep it in mem
static WorldRef World0Ref;

void Server::start() {
	WorldRef wr = G.U->createWorld(0);
	World0Ref = wr;

	//G.SC->save("/tmp/a");
	//G.SC->load("/tmp/a");
}

void Server::stop() {
	
}

void Server::stopInternals() {
	lua_close(LS);
}

void Server::chunkUpdater(WorldRef WR, bool &continueUpdate) {
	World &W = *WR;
	while (continueUpdate) {
		ChunkRef c;
		for (auto pair : W)
			if ((c = pair.second.lock()))
				c->updateServerPrepare();
		for (auto pair : W)
			if ((c = pair.second.lock()))
				c->updateServer();
		for (auto pair : W)
			if ((c = pair.second.lock()))
				c->updateServerSwap();
		for (auto pair : W) {
			if ((c = pair.second.lock()) && !c->CH.empty()) {
				// TODO: view range
				OutMessage msg(MessageType::ChunkUpdate, 0);
				glm::ivec3 pos = c->getWorldChunkPos();
				msg.writeI16(pos.x);
				msg.writeI16(pos.y);
				msg.writeI16(pos.z);
				c->CH.flush(msg);
				NetHelper::Broadcast(G, msg, Tfer::Rel, Channels::MapUpdate);
			}
		}
		for (Player &p : G.players) {
			for (auto it = p.pendingChunks.begin(); it != p.pendingChunks.end(); ++it) {
				if ((*it)->state == Chunk::State::Ready) {
					glm::ivec3 pos = (*it)->getWorldChunkPos();
					getDebugStream() << "Send pent Chunk[" << pos.x << ',' << pos.y << ' ' << pos.z <<
						"] send to " << p.name << std::endl;
					sendChunk(**it, p);
					it = p.pendingChunks.erase(it)--;
				}
			}
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
}

void Server::run() {
	InMessage msg;
	Peer peer;
	bool continueUpdate = true;
	std::thread upd(&Server::chunkUpdater, this, G.U->getWorld(0), std::ref(continueUpdate));
	Player *plr;
	while (true) {
		if (H.recv(msg, peer, 100)) {
			plr = getPlayerByPeer(peer);
			if (plr != nullptr) {
				switch (msg.getType()) {
					case MessageType::Chat:
					handleChat(msg, plr);
					break;
				case MessageType::PlayerUpdate:
					handlePlayerUpdate(msg, *plr);
					break;
				case MessageType::Event:
					handleEvent(msg, peer);
					break;
				case MessageType::ChunkTransfer:
					handlePlayerChunkRequest(msg, *plr);
					break;
				case MessageType::ChunkUpdate:
					handlePlayerMapUpdate(msg, *plr);
					break;
				
				default:
					break;
				}
			}
			switch (msg.getType()) {
			case MessageType::Connect:
				getOutputStream() << peer.getHost() << " NEWCONN" << std::endl;
				break;
			case MessageType::Disconnect:
				handleDisconnect(peer);
				break;
			
			case MessageType::PlayerJoin:
				handlePlayerJoin(msg, peer);
				break;
			case MessageType::PlayerQuit:
				handlePlayerQuit(peer);
				break;
			
			default:
				break;
			}
		}
	}
	continueUpdate = false;
	upd.join();
	getDebugStream() << "chunk updater thread joined" << std::endl;
}

bool Server::isPlayerOnline(const std::string &playername) const {
	for (const Player &p : G.players) {
		if (p.name == playername)
			return true;
	}
	return false;
}

void Server::kick(Player &p, Net::QuitReason r, const std::string &message) {
	OutMessage msg(MessageType::PlayerQuit, r);
	msg.writeU32(p.id);
	msg.writeString(message);
	H.send(p.P, msg, Tfer::Rel);
	p.P.disconnect();
}

Server::~Server() {
}

}