#include "Server.hpp"
#include "Game.hpp"
#include "network/msgtypes/BlockUpdate.hpp"
#include "network/msgtypes/Chat.hpp"
#include "network/msgtypes/ChunkTransfer.hpp"
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
  plr.W = G.U->getLoadWorld(0);

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
  for (int x = -2; x < 2; ++x)
    for (int y = -2; y < 2; ++y)
      for (int z = -2; z < 2; ++z)
        schedSendChunk(G.U->getWorld(0)->getLoadChunk(x, y, z), plr);
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
  using namespace Net::MsgTypes;
  using S = ChatSubtype;
  switch (static_cast<S>(msg.getSubtype())) {
    case S::Send: {
      ChatSend cs;
      cs.readFromMsg(msg);

      ChatPlayerTalk cpt;
      cpt.player.id = plr->id;
      cpt.msg = cs.msg;
      OutMessage omsg;
      cpt.writeToMsg(omsg);
      NetHelper::Broadcast(G, omsg, Tfer::Rel);
    } break;
    case S::Announcement:
    case S::PlayerTalk:
      ; // No-op
      break;
  }
}

void Server::handleEvent(InMessage &msg, Peer &peer) {
  (void) msg; (void) peer;
  // Player &plr = G.players.getByPeer(peer);
  // switch (msg.getSubtype())
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
  P.pendingChunks.emplace_back(C);
}

void Server::sendChunks(const std::list<ChunkRef> &cs, Player &P) {
  using namespace Net::MsgTypes;
  ChunkTransferResponse ctr;
  std::vector<OutMemoryStream> chunkBufs(cs.size());
  size_t i = 0;
  for (const ChunkRef &cr : cs) {
    ctr.chunks.emplace_back();
    ChunkTransferResponse::ChunkData &cd = ctr.chunks.back();
    const Chunk &c = *cr;
    cd.worldId = c.getWorld()->id;
    cd.chunkPos = c.getWorldChunkPos();
    c.write(chunkBufs[i]);
    cd.dataLength = chunkBufs[i].length();
    cd.data = chunkBufs[i].data();
    ++i;
  }
  OutMessage msg;
  ctr.writeToMsg(msg);
  H.send(P.P, msg, Tfer::Rel, Channels::MapTransfer);
}

void Server::handlePlayerChunkRequest(InMessage &msg, Player &plr) {
  using namespace Net::MsgTypes;
  using S = ChunkTransferSubtype;
  switch (static_cast<S>(msg.getSubtype())) {
    case S::Request: {
      // TODO: distance & tool check, i.e. legitimate update
      ChunkTransferRequest ctr;
      ctr.readFromMsg(msg);
      for (const ChunkTransferRequest::ChunkData &cd : ctr.chunks) {
        WorldRef wld = G.U->getWorld(cd.worldId);
        if (wld) {
          schedSendChunk(wld->getLoadChunk(cd.chunkPos.x, cd.chunkPos.y, cd.chunkPos.z), plr);
        }
      }
    } break;
    case S::Response:
    case S::Denied: {
      ; // No-op
    } break;
  }
}

void Server::handlePlayerMapUpdate(InMessage &msg, Player &plr) {
  // TODO: distance & tool check, i.e. legitimate update
  using namespace Net::MsgTypes;
  using S = BlockUpdateSubtype;
  switch (static_cast<S>(msg.getSubtype())) {
    case S::Notify: {
      ; // No-op
    } break;
    case S::Place: {
      BlockUpdatePlace bup;
      bup.readFromMsg(msg);
      //TODO
    } break;
    case S::Break: {
      BlockUpdateBreak bub;
      bub.readFromMsg(msg);
      WorldRef w = G.U->getWorld(bub.worldId);
      if (w) {
        ChunkRef c = w->getChunkAtCoords(bub.pos);
        if (c) {
          c->setBlock(rmod(bub.pos.x, CX), rmod(bub.pos.y, CY), rmod(bub.pos.z, CZ), 0, 0);
          if (!c->CH.empty()) {
            BlockUpdateNotify bun;
            c->CH.flush(bun);
            OutMessage omsg;
            bun.writeToMsg(omsg);
            NetHelper::Broadcast(G, omsg, Tfer::Rel, Channels::MapUpdate);
          }
        }
      }
    } break;
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
  L = luaL_newstate();
  luaL_openlibs(L);
  luaL_loadfile(L, "");
  lua_pcall(L, 0, 0, 0);
}

// FIXME ugly ugly hack to keep it in mem
static WorldRef World0Ref;

void Server::start() {
  // TODO: loading
  G.U = new Universe(&G, false);

  WorldRef wr = G.U->createWorld(0);
  World0Ref = wr;

  for (int x = -2; x < 2; ++x)
    for (int y = -2; y < 2; ++y)
      for (int z = -2; z < 2; ++z)
        holdChunksInMem.emplace_back(G.U->getWorld(0)->getLoadChunk(x, y, z));
}

void Server::stop() {
  
}

void Server::stopInternals() {
  lua_close(L);
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
        Net::MsgTypes::BlockUpdateNotify bun;
        c->CH.flush(bun);
        OutMessage msg;
        bun.writeToMsg(msg);
        NetHelper::Broadcast(G, msg, Tfer::Rel, Channels::MapUpdate);
      }
    }
    std::list<ChunkRef> chunksToSend;
    for (Player &p : G.players) {
      chunksToSend.clear();
      for (auto it = p.pendingChunks.begin();
           it != p.pendingChunks.end() && chunksToSend.size() < 32; ++it) {
        if ((*it)->getState() == Chunk::State::Ready) {
          chunksToSend.push_back(std::move(*it));
          it = --p.pendingChunks.erase(it);
        }
      }
      sendChunks(chunksToSend, p);
    }
    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
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
        case MessageType::BlockUpdate:
          handlePlayerMapUpdate(msg, *plr);
          break;
        
        default:
          break;
        }
      }
      switch (msg.getType()) {
      case MessageType::NetConnect:
        getOutputStream() << peer.getHost() << " NEWCONN" << std::endl;
        break;
      case MessageType::NetDisconnect:
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
