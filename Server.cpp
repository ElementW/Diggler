#include "Server.hpp"
#include "Game.hpp"
#include "network/Network.hpp"
#include "network/NetHelper.hpp"
#include "VersionInfo.hpp"
#include "CaveGenerator.hpp"
#include "ChunkChangeHelper.hpp"
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
		return &G->players.getByPeer(peer);
	} catch (const std::out_of_range &e) {
		return nullptr;
	}
}
Player* Server::getPlayerById(uint32 id) {
	try {
		return &G->players.getById(id);
	} catch (const std::out_of_range &e) {
		return nullptr;
	}
}
Player* Server::getPlayerByName(const std::string &name) {
	try {
		return &G->players.getByName(name);
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
	
	Player &plr = G->players.add();
	plr.name = name;
	plr.id = FastRand();
	plr.P = peer;
	
	// Confirm successful join
	OutMessage join(MessageType::PlayerJoin);
	join.writeU32(plr.id);
	H.send(peer, join, Tfer::Rel);
	
	// Send the player list
	for (Player &p : G->players) {
		if (p.id == plr.id)
			continue; // ok, he knows he's here
		OutMessage playerMsg(MessageType::PlayerJoin);
		playerMsg.writeU32(p.id);
		playerMsg.writeString(p.name);
		H.send(peer, playerMsg, Tfer::Rel);
	}
	
	OutMessage map(MessageType::MapTransfer);
	G->SC->writeMsg(map);
	H.send(peer, map, Tfer::Rel);
	
	// Broadcast player's join
	OutMessage broadcast(MessageType::PlayerJoin);
	broadcast.writeU32(plr.id);
	broadcast.writeString(plr.name);
	for (Player &p : G->players) {
		if (p.id == plr.id)
			continue; // dont send broadcast to the player
		H.send(p.P, broadcast, Tfer::Rel);
	}
	getOutputStream() << plr.name << " joined from " << peer.getHost() << endl;
}

void Server::handlePlayerQuit(Peer &peer, QuitReason reason) {
	Player *plrPtr = getPlayerByPeer(peer);
	if (plrPtr) {
		Player &plr = *plrPtr;
		// Broadcast disconnection
		OutMessage broadcast(MessageType::PlayerQuit, reason);
		broadcast.writeU32(plr.id);
		for (Player &p : G->players) {
			if (p.id == plr.id)
				continue; // dont send broadcast to the player
			H.send(p.P, broadcast, Tfer::Rel);
		}
		getOutputStream() << plr.name << " disconnected" << endl;
		G->players.remove(plr);
	} else {
		getOutputStream() << peer.getHost() << " disconnected" << endl;
	}
}

void Server::handleDisconnect(Peer &peer) {
	handlePlayerQuit(peer, QuitReason::Timeout);
}

void Server::handleEvent(InMessage &msg, Peer &peer) {
	Player &plr = G->players.getByPeer(peer);
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

void Server::handleChat(InMessage &msg, Peer &peer) {
	try {
		// TODO: implement codecvt_utf8<utf32> when libstdc++ supports it
		Player &plr = G->players.getByPeer(peer);
		std::string chatMsg = msg.readString();
		getOutputStream() << plr.name << ": " << chatMsg << endl;
		std::ostringstream contentFormatter;
		contentFormatter << plr.name << "> " << chatMsg;
		std::string content = contentFormatter.str();
		OutMessage newMsg(MessageType::Chat);
		newMsg.writeString(content);
		NetHelper::Broadcast(G, newMsg, Tfer::Rel);
	} catch (const std::out_of_range &e) {
		getErrorStream() << peer.getHost() << " sent chat message but is not connected" << std::endl;
	}
}

void Server::handlePlayerUpdate(InMessage &msg, Peer &peer) {
	try {
		Player &plr = G->players.getByPeer(peer);
		
		switch (msg.getSubtype()) {
		case PlayerUpdateType::Move: {
			// Broadcast movement
			OutMessage bcast(MessageType::PlayerUpdate, PlayerUpdateType::Move);
			bcast.writeU32(plr.id);
			glm::vec3 pos = msg.readVec3(),
					vel = msg.readVec3(),
					acc = msg.readVec3();
			bcast.writeVec3(pos);
			bcast.writeVec3(vel);
			bcast.writeVec3(acc);
			for (Player &p : G->players) {
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
	} catch (std::out_of_range &e) {
		// TODO: log?
		return;
	}
}

void Server::handlePlayerMapUpdate(InMessage &msg, Peer &peer) {
	// TODO: distance & tool check, i.e. legitimate update
	int x = msg.readU16(), y = msg.readU16(), z = msg.readU16();
	BlockType b = (BlockType)msg.readU8();
	G->SC->set(x, y, z, b);
	// FIXME: v This might interfere with the ticker
	if (!G->CCH->empty()) {
		OutMessage msg(MessageType::MapUpdate, G->CCH->count());
		G->CCH->flush(msg);
		NetHelper::Broadcast(G, msg, Tfer::Rel, Channels::MapUpdate);
	}
}

void Server::handlePlayerDeath(InMessage &msg, Player &plr) {
	uint8 drb = msg.readU8();
	Player::DeathReason dr = (Player::DeathReason)drb;
	plr.setDead(false, dr);
	OutMessage out(MessageType::PlayerUpdate, PlayerUpdateType::Die);
	out.writeU32(plr.id);
	out.writeU8(drb);
	for (Player &p : G->players) {
		if (p.id != plr.id)
			H.send(p.P, out, Tfer::Rel, Channels::Life);
	}
	
	// Respawn player later
	Game *G = this->G; uint32 id = plr.id;
	std::thread respawn([G, id] {
		getDebugStream() << "Respawn " << id << " in 2 secs " << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() << std::endl;
		std::this_thread::sleep_for(std::chrono::seconds(2));
		getDebugStream() << "Respawn " << id << " in 2 secs " << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::system_clock::now().time_since_epoch()).count() << std::endl;
		Player *plr = G->S->getPlayerById(id);
		if (plr) {
			plr->setDead(false);
			OutMessage out(MessageType::PlayerUpdate, PlayerUpdateType::Die);
			out.writeU32(id);
			NetHelper::Broadcast(G, out, Tfer::Rel, Channels::Life);
		}
	});
	respawn.detach();
}

Server::Server(Game *G, uint16 port) : G(G) {
	G->init();

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

#if 1
	G->SC->setSize(4, 4, 4);

	//for (int i=0; i < 8192; i++) G->SC->set(FastRand(CX*G->SC->getChunksX()), FastRand(CY*G->SC->getChunksY()), FastRand(CZ*G->SC->getChunksZ()), (BlockType)(FastRand((int)BlockType::LAST)));
	for(int x=0;x<CX*G->SC->getChunksX();x++) for(int z=0;z<(CZ*G->SC->getChunksZ())/2;z++) G->SC->set(x, 0, z, BlockType::Dirt);
	for(int x=0;x<CX*G->SC->getChunksX();x++) for(int z=0;z<(CZ*G->SC->getChunksZ())/2;z++) G->SC->set(x, 0, z+(CZ*G->SC->getChunksZ())/2, BlockType::Road);
	//	for(int x=0;x<CX*G->SC->getChunksX();x++) for(int y=0;y<16;y++) for(int z=0;z<CZ*G->SC->getChunksZ();z++) G->SC->set(x,y,z,BlockType::Dirt);
	for(int x=0; x < (int)BlockType::LAST; x++) G->SC->set(x, 2, 0, (BlockType)(x));
	G->SC->set(4, 4, 4, BlockType::Shock);
	G->SC->set(4, 0, 4, BlockType::Jump);
	
	G->SC->set(0, 1, 1, BlockType::Metal);
	G->SC->set(0, 2, 1, BlockType::Metal);
	G->SC->set(0, 3, 1, BlockType::Metal);
	
	G->SC->set(1, 3, 1, BlockType::Metal);
	G->SC->set(2, 3, 1, BlockType::Metal);
	
	G->SC->set(3, 1, 1, BlockType::Metal);
	G->SC->set(3, 2, 1, BlockType::Metal);
	G->SC->set(3, 3, 1, BlockType::Metal);
	
	CaveGenerator::PaintAtPoint(*(G->SC), 8, 8, 8, 1, BlockType::Dirt);
	CaveGenerator::PaintAtPoint(*(G->SC), 16, 8, 8, 2, BlockType::Dirt);
	CaveGenerator::PaintAtPoint(*(G->SC), 24, 8, 8, 3, BlockType::Dirt);
	
	for(int x=0;x<CX*G->SC->getChunksX();x++) for(int z=0;z<(CZ*G->SC->getChunksZ())/2;z++) G->SC->set(x, 64, z, BlockType::Dirt);
	G->SC->set(2*CX, 68, 2*CY, BlockType::Lava);
#else
	G->SC->setSize(4, 4, 4);
	CaveGenerator::GenerateCaveSystem(*(G->SC), true, 15);
#endif

	//G->SC->save("/tmp/a");
	//G->SC->load("/tmp/a");
	
	/*{
		Game *G = this->G;
		std::thread make([G]{CaveGenerator::GenerateCaveSystem(*(G->SC), true, 15);});
		make.detach();
	}*/
}

void chunk_updater(Game *G, Superchunk *sc, Host &H) {
	while (true) {
		for (int x=0; x < CX; x++)
			for (int y=0; y < CY; y++)
				for (int z=0; z < CZ; z++) {
					Chunk* c = sc->getChunk(x, y, z);
					if (c)
						c->updateServerPrepare();
				}
		for (int x=0; x < CX; x++)
			for (int y=0; y < CY; y++)
				for (int z=0; z < CZ; z++) {
					Chunk* c = sc->getChunk(x, y, z);
					if (c)
						c->updateServer();
				}
		for (int x=0; x < CX; x++)
			for (int y=0; y < CY; y++)
				for (int z=0; z < CZ; z++) {
					Chunk* c = sc->getChunk(x, y, z);
					if (c)
						c->updateServerSwap();
				}
		if (!G->CCH->empty()) {
			OutMessage msg(MessageType::MapUpdate, G->CCH->count());
			// Message subtype = update count, trickery ;)
			G->CCH->flush(msg);
			NetHelper::Broadcast(G, msg, Tfer::Rel, Channels::MapUpdate);
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
}

void Server::run() {
	InMessage msg;
	Peer peer;
	std::thread upd(chunk_updater, G, G->SC.get(), std::ref(H));
	while (true) {
		if (H.recv(msg, peer, 100)) {
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
			
			case MessageType::Chat:
				handleChat(msg, peer);
				break;
			case MessageType::PlayerUpdate:
				handlePlayerUpdate(msg, peer);
				break;
			case MessageType::Event:
				handleEvent(msg, peer);
				break;
			case MessageType::MapUpdate:
				handlePlayerMapUpdate(msg, peer);
				break;
			
			default:
				break;
			}
		}
	}
}

bool Server::isPlayerOnline(const std::string &playername) const {
	for (const Player &p : G->players) {
		if (p.name == playername)
			return true;
	}
	return false;
}

void Server::kick(Player& p, Net::QuitReason r, const std::string& message) {
	OutMessage msg(MessageType::PlayerQuit, r);
	msg.writeU32(p.id);
	msg.writeString(message);
	H.send(p.P, msg, Tfer::Rel);
	p.P.disconnect();
}

Server::~Server() {
}

}