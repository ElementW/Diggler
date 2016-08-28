#include "ClientMessageHandler.hpp"

#include "../Chatbox.hpp"
#include "../Game.hpp"
#include "../GameState.hpp"
#include "msgtypes/BlockUpdate.hpp"
#include "msgtypes/Chat.hpp"
#include "msgtypes/ChunkTransfer.hpp"
#include "msgtypes/PlayerUpdate.hpp"

namespace Diggler {
namespace Net {

ClientMessageHandler::ClientMessageHandler(GameState &gameState) :
  GS(gameState) {
}

bool ClientMessageHandler::handleMessage(InMessage &msg) {
  using namespace Net::MsgTypes;
  switch (msg.getType()) {
    case MessageType::NetDisconnect:
      GS.GW->showMessage("Disconnected", "Timed out");
      return false;

    case MessageType::ChunkTransfer: {
      using S = ChunkTransferSubtype;
      switch (static_cast<S>(msg.getSubtype())) {
        case S::Request: {
          ; // No-op
        } break;
        case S::Response: {
          ChunkTransferResponse ctr;
          ctr.readFromMsg(msg);
          for (const ChunkTransferResponse::ChunkData &cd : ctr.chunks) {
            ChunkRef c = GS.G->U->getLoadWorld(cd.worldId)->getNewEmptyChunk(
              cd.chunkPos.x, cd.chunkPos.y, cd.chunkPos.z);
            InMemoryStream ims(cd.data, cd.dataLength);
            c->read(ims);
            GS.holdChunksInMem.push_back(c);
          }
        } break;
        case S::Denied: {
          ChunkTransferDenied ctd;
          ctd.readFromMsg(msg);
        } break;
      }
    } break;
    case MessageType::Chat: {
      using S = ChatSubtype;
      switch (static_cast<S>(msg.getSubtype())) {
        case S::Send: {
          ; // No-op
        } break;
        case S::Announcement: {
          ChatAnnouncement ca;
          ca.readFromMsg(msg);
          // TODO better formatting abilities
          if (ca.msg.type == msgpack::type::STR) {
            GS.m_chatBox->addChatEntry(ca.msg.as<std::string>());
          }
        } break;
        case S::PlayerTalk: {
          ChatPlayerTalk cpt;
          cpt.readFromMsg(msg);
          // TODO better formatting abilities
          if (cpt.msg.type == msgpack::type::STR) {
            std::string playerName;
            if (cpt.player.display.type == msgpack::type::NIL) {
              const Player *blabbermouth = GS.G->players.getBySessId(cpt.player.id);
              if (blabbermouth != nullptr) {
                playerName = blabbermouth->name + "> ";
              } else {
                playerName = "?> ";
              }
            } else if (cpt.player.display.type == msgpack::type::STR) {
              cpt.player.display.convert(playerName);
            }
            GS.m_chatBox->addChatEntry(playerName + cpt.msg.as<std::string>());
          }
        } break;
      }
    } break;
    case MessageType::PlayerJoin: {
      Player &plr = GS.G->players.add();
      plr.sessId = msg.readU32();
      plr.name = msg.readString();
      getDebugStream() << "Player " << plr.name << '(' << plr.sessId << ") joined the party!" << std::endl;
    } break;
    case MessageType::PlayerQuit: {
      uint32 id = msg.readU32();
      try {
        Player *plr = GS.G->players.getBySessId(id);
        if (plr != nullptr) {
          getOutputStream() << plr->name << " is gone :(" << std::endl;
          GS.G->players.remove(*plr);
        }
      } catch (const std::out_of_range &e) {
        getOutputStream() << "Phantom player #" << id << " disconnected" << std::endl;
      }
    } break;
    case MessageType::PlayerUpdate: {
      using S = PlayerUpdateSubtype;
      switch (static_cast<S>(msg.getSubtype())) {
        case S::Move: {
          PlayerUpdateMove pum;
          pum.readFromMsg(msg);
          if (!pum.plrSessId) {
            getOutputStream() << "PlayerUpdateMove without player session ID" << std::endl;
            return true;
          }
          Player *plr = GS.G->players.getBySessId(*pum.plrSessId);
          if (!plr) {
            getOutputStream() << "PlayerUpdateMove: sess#" << *pum.plrSessId <<
              " is not on server" << std::endl;
            return true;
          }
          glm::vec3 pos = pum.position ? *pum.position : plr->position,
                    vel = pum.velocity ? *pum.velocity : plr->velocity,
                    acc = pum.accel ? *pum.accel : plr->accel;
          plr->setPosVel(pos, vel, acc);
          plr->angle = pum.angle;
        } break;
        case S::Die: {
          PlayerUpdateDie pud;
          pud.readFromMsg(msg);
          Player *plr = GS.G->players.getBySessId(pud.plrSessId);
          if (!plr) {
            getOutputStream() << "PlayerUpdateDie: sess#" << pud.plrSessId <<
              " is not on server" << std::endl;
            return true;
          }
          plr->setDead(false, pud.reason);
        } break;
        case S::Respawn: {
          PlayerUpdateRespawn pur;
          pur.readFromMsg(msg);
          Player *plr = GS.G->players.getBySessId(pur.plrSessId);
          if (!plr) {
            getOutputStream() << "PlayerUpdateRespawn: sess#" << pur.plrSessId <<
              " is not on server" << std::endl;
            return true;
          }
          plr->setDead(false);
        } break;
        default:
          break;
      }
    } break;
    case MessageType::BlockUpdate: {
      // TODO handle that in Chunk's ChangeHelper
      using S = BlockUpdateSubtype;
      switch (static_cast<S>(msg.getSubtype())) {
        case S::Notify: {
          BlockUpdateNotify bun;
          bun.readFromMsg(msg);
          for (const BlockUpdateNotify::UpdateData &upd : bun.updates) {
            WorldRef w = GS.G->U->getWorld(upd.worldId);
            if (w) {
              ChunkRef c = w->getChunkAtCoords(upd.pos);
              if (c) {
                c->setBlock(rmod(upd.pos.x, CX), rmod(upd.pos.y, CY), rmod(upd.pos.z, CZ),
                            upd.id, upd.data);
                // TODO extdata & light
              }
            }
          }
        } break;
        case S::Place:
        case S::Break: {
          ; // No-op
        } break;
      }
    } break;

    case MessageType::ServerInfo:
    case MessageType::NetConnect:
    case MessageType::Null:
    default: {
      ; // No-op
    } break;
  }
  return true;
}

}
}
