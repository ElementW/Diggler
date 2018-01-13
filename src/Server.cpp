#include "Server.hpp"

#include <algorithm>
#include <iterator>
#include <thread>
#include <sstream>

#include <lua.h>

#include "Game.hpp"
#include "network/msgtypes/BlockUpdate.hpp"
#include "network/msgtypes/Chat.hpp"
#include "network/msgtypes/ChunkTransfer.hpp"
#include "network/msgtypes/Content.hpp"
#include "network/msgtypes/PlayerJoin.hpp"
#include "network/msgtypes/PlayerUpdate.hpp"
#include "network/Network.hpp"
#include "network/NetHelper.hpp"
#include "scripting/lua/State.hpp"
#include "VersionInfo.hpp"
#include "CaveGenerator.hpp"
#include "util/Log.hpp"

using std::cout;
using std::endl;

using namespace diggler::net;

namespace diggler {

using Util::Log;
using namespace Util::Logging::LogLevels;

static const char *TAG = "Server";

inline Player* Server::getPlayerByPeer(const Peer &peer) {
  return G.players.getByPeer(peer);
}
inline Player *Server::getPlayerBySessId(uint32 id) {
  return G.players.getBySessId(id);
}
inline Player* Server::getPlayerByName(const std::string &name) {
  return G.players.getByName(name);
}

void Server::handlePlayerJoin(InMessage &msg, Peer &peer) {
  using PJS = MsgTypes::PlayerJoinSubtype;
  if (msg.getSubtype<PJS>() != PJS::Request) {
    return;
  }

  MsgTypes::PlayerJoinRequest pjr; pjr.readFromMsg(msg);
  Log(Info, TAG) << peer.peerHost() << " is joining as " << pjr.name;
  
  // TODO: ban list
  Player *possible = getPlayerByName(pjr.name);
  if (possible != nullptr) {
    MsgTypes::PlayerJoinFailure pjf;
    pjf.failureReason = MsgTypes::PlayerJoinFailure::FailureReason::UsernameAlreadyUsed;
    OutMessage pjfMsg; pjf.writeToMsg(pjfMsg);
    //kick.writeString("You are \faalready\f0 playing on this server");
    H.send(peer, pjfMsg, Tfer::Rel);
    Log(Verbose, TAG) << peer.peerHost() << " tried to connect as " << pjr.name << ": name is taken";
    return;
  }

  Player &plr = G.players.add();
  plr.name = pjr.name;
  plr.sessId = FastRand();
  plr.peer = &peer;
  plr.W = G.U->getLoadWorld(0);

  /* Confirm successful join */ {
    MsgTypes::PlayerJoinSuccess pjs;
    pjs.sessId = plr.sessId;
    pjs.wid = plr.W->id;
    OutMessage join; pjs.writeToMsg(join);
    H.send(peer, join, Tfer::Rel);
  }

  /* Send the player list */ {
    MsgTypes::PlayerJoinBroadcast pjb;
    pjb.joinReason = MsgTypes::PlayerJoinBroadcast::JoinReason::AlreadyPlaying;
    for (Player &p : G.players) {
      if (p.sessId == plr.sessId)
        continue;
      pjb.sessId = p.sessId;
      pjb.name = p.name;
      pjb.wid = p.W->id;
      OutMessage playerMsg; pjb.writeToMsg(playerMsg);
      H.send(peer, playerMsg, Tfer::Rel);
    }
  }

  /* Broadcast player's join */ {
    MsgTypes::PlayerJoinBroadcast pjb;
    pjb.sessId = plr.sessId;
    pjb.name = plr.name;
    pjb.wid = plr.W->id;
    OutMessage broadcast; pjb.writeToMsg(broadcast);
    for (Player &p : G.players) {
      if (p.sessId == plr.sessId)
        continue; // don't send broadcast to the player
      H.send(*p.peer, broadcast, Tfer::Rel);
    }
  }

  Log(Verbose, TAG) << plr.name << " joined from " << peer.peerHost();
  for (int x = -2; x < 2; ++x)
    for (int y = -2; y < 2; ++y)
      for (int z = -2; z < 2; ++z)
        schedSendChunk(G.U->getWorld(0)->getChunk(x, y, z, true), plr);
}

void Server::handlePlayerQuit(Peer &peer, QuitReason reason) {
  Player *plrPtr = getPlayerByPeer(peer);
  if (plrPtr) {
    Player &plr = *plrPtr;
    // Broadcast disconnection
    OutMessage broadcast(MessageType::PlayerQuit, reason);
    broadcast.writeU32(plr.sessId);
    for (Player &p : G.players) {
      if (p.sessId == plr.sessId)
        continue; // dont send broadcast to the player
      H.send(*p.peer, broadcast, Tfer::Rel);
    }
    Log(Verbose, TAG) << plr.name << " disconnected";
    G.players.remove(plr);
  } else {
    Log(Verbose, TAG) << peer.peerHost() << " disconnected";
  }
}

void Server::handleDisconnect(Peer &peer) {
  handlePlayerQuit(peer, QuitReason::Timeout);
}

void Server::handleContentMessage(net::InMessage &msg, net::Peer &peer) {
  using namespace net::MsgTypes;
  using S = ContentSubtype;
  switch (msg.getSubtype<MsgTypes::ContentSubtype>()) {
  case S::ModListRequest: {
    ContentModListResponse cmlr;
    // cmlr.modsIds // TODO fill in
    OutMessage omsg;
    cmlr.writeToMsg(omsg);
    H.send(peer, omsg);
  } break;
  default:
    break;
  }
}

void Server::handleChat(InMessage &msg, Player *plr) {
  using namespace net::MsgTypes;
  using S = ChatSubtype;
  switch (msg.getSubtype<S>()) {
    case S::Send: {
      ChatSend cs;
      cs.readFromMsg(msg);

      ChatPlayerTalk cpt;
      cpt.player.id = plr->sessId;
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

void Server::handleCommand(Player *plr, const std::string &command, const std::vector<std::string> &args) {
  Log(Verbose, TAG) << "Command \"" << command << '"';
}

void Server::handlePlayerUpdate(InMessage &msg, Player &plr) {
  using namespace net::MsgTypes;
  using S = PlayerUpdateSubtype;
  switch (msg.getSubtype<S>()) {
  case S::Move: {
    PlayerUpdateMove pum;
    pum.readFromMsg(msg);
    pum.plrSessId = plr.sessId;
    // Broadcast movement
    OutMessage bcast; pum.writeToMsg(bcast);
    for (Player &p : G.players) {
      if (p.sessId == plr.sessId)
        continue; // dont send broadcast to the player
      // TODO: confirm position to player
      H.send(*p.peer, bcast, Tfer::Unrel);
    }
  } break;
  case S::Die:
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
  using namespace net::MsgTypes;
  ChunkTransferResponse ctr;
  std::vector<io::OutMemoryStream> chunkBufs(cs.size());
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
  H.send(*P.peer, msg, Tfer::Rel, Channels::MapTransfer);
}

void Server::handlePlayerChunkRequest(InMessage &msg, Player &plr) {
  using namespace net::MsgTypes;
  using S = ChunkTransferSubtype;
  switch (msg.getSubtype<S>()) {
    case S::Request: {
      // TODO: distance & tool check, i.e. legitimate update
      ChunkTransferRequest ctr;
      ctr.readFromMsg(msg);
      for (const ChunkTransferRequest::ChunkData &cd : ctr.chunks) {
        WorldRef wld = G.U->getWorld(cd.worldId);
        if (wld) {
          schedSendChunk(wld->getChunk(cd.chunkPos.x, cd.chunkPos.y, cd.chunkPos.z, true), plr);
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
  using namespace net::MsgTypes;
  using S = BlockUpdateSubtype;
  constexpr auto CX = Chunk::CX, CY = Chunk::CY, CZ = Chunk::CZ;
  switch (msg.getSubtype<S>()) {
    case S::Notify: {
      ; // No-op
    } break;
    case S::Place: {
      BlockUpdatePlace bup;
      bup.readFromMsg(msg);
      WorldRef w = G.U->getWorld(bup.worldId);
      if (w) {
        ChunkRef c = w->getChunkAtCoords(bup.pos);
        if (c) {
          c->setBlock(rmod(bup.pos.x, CX), rmod(bup.pos.y, CY), rmod(bup.pos.z, CZ),
                      bup.id, bup.data);
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
    case S::Break: {
      BlockUpdateBreak bub;
      bub.readFromMsg(msg);
      WorldRef w = G.U->getWorld(bub.worldId);
      if (w) {
        ChunkRef c = w->getChunkAtCoords(bub.pos);
        if (c) {
          c->setBlock(rmod(bub.pos.x, CX), rmod(bub.pos.y, CY), rmod(bub.pos.z, CZ),
                      content::BlockAirId, 0);
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
  using namespace net::MsgTypes;
  PlayerUpdateDie pud;
  pud.readFromMsg(msg);
  pud.plrSessId = plr.sessId;
  plr.setDead(false);
  OutMessage out; pud.writeToMsg(out);
  for (Player &p : G.players) {
    if (p.sessId != plr.sessId)
      H.send(*p.peer, out, Tfer::Rel, Channels::Life);
  }
  
  // Respawn player later
  Game *G = &this->G; Player::SessionID sid = plr.sessId;
  std::thread respawn([G, sid] {
    std::this_thread::sleep_for(std::chrono::seconds(2));
    Player *plr = G->S->getPlayerBySessId(sid);
    if (plr) {
      plr->setDead(false);
      PlayerUpdateRespawn pur;
      pur.plrSessId = sid;
      OutMessage out; pur.writeToMsg(out);
      NetHelper::Broadcast(G, out, Tfer::Rel, Channels::Life);
    }
  });
  respawn.detach();
}

Server::Server(Game &G, uint16 port) : G(G) {
  G.init();

  Log(Info, TAG) << "Diggler v" << VersionString << " Server, port " << port << ", " <<
      std::thread::hardware_concurrency() << " HW threads supported";

  if (port >= 49152) {
    Log(Warning, TAG) << "port is within the Ephemeral Port Range as defined by IANA!\n" <<
        "Nothing wrong with that, but for compatibility's sake please avoid this range.";
  }

  try {
    H.create(port);
  } catch (net::Exception &e) {
    Log(Error, TAG) << "Couldn't open port " << port << " for listening\n" <<
        "Make sure no other server instance is running";
    if (port <= 1024) {
      Log(Warning, TAG) << "The selected port is in range 1-1024, which typically require root "
        "privileges. Make sure you have permission to bind to this port.";
    }
    throw "Server init failed";
  }

  startInternals();
  start();
}

void Server::startInternals() {
  G.LS->initialize();
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
        holdChunksInMem.emplace_back(G.U->getWorld(0)->getChunk(x, y, z, true));
}

void Server::stop() {
  
}

void Server::stopInternals() {
  G.LS->finalize();
}

void Server::chunkUpdater(WorldRef WR, bool &continueUpdate) {
  World &W = *WR;
  while (continueUpdate) {
    ChunkRef c;
    for (auto pair : W)
      if ((c = pair.second.lock()))
        c->updateServer();
    for (auto pair : W) {
      if ((c = pair.second.lock()) && !c->CH.empty()) {
        // TODO: view range
        net::MsgTypes::BlockUpdateNotify bun;
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
  Peer *peerPtr;
  bool continueUpdate = true;
  std::thread upd(&Server::chunkUpdater, this, G.U->getWorld(0), std::ref(continueUpdate));
  Player *plr;
  while (true) {
    if (H.recv(msg, &peerPtr, 100)) {
      Peer &peer = *peerPtr;
      plr = getPlayerByPeer(peer);
      if (plr != nullptr) {
        switch (msg.getType()) {
          case MessageType::Chat:
          handleChat(msg, plr);
          break;
        case MessageType::PlayerUpdate:
          handlePlayerUpdate(msg, *plr);
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
        Log(Debug, TAG) << peer.peerHost() << " NEWCONN";
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

      case MessageType::Content:
        handleContentMessage(msg, peer);
        break;

      default:
        break;
      }
    }
  }
  continueUpdate = false;
  upd.join();
  Log(Debug, TAG) << "chunk updater thread joined";
}

bool Server::isPlayerOnline(const std::string &playername) const {
  for (const Player &p : G.players) {
    if (p.name == playername)
      return true;
  }
  return false;
}

void Server::kick(Player &p, net::QuitReason r, const std::string &message) {
  OutMessage msg(MessageType::PlayerQuit, r);
  msg.writeU32(p.sessId);
  msg.writeString(message);
  H.send(*p.peer, msg, Tfer::Rel);
  p.peer->disconnect();
}

Server::~Server() {
}

}
