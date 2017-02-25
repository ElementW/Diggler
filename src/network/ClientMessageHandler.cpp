#include "ClientMessageHandler.hpp"

#include "../Chatbox.hpp"
#include "../Game.hpp"
#include "../GameState.hpp"
#include "client/BlockUpdateHandler.hpp"
#include "client/ContentHandler.hpp"
#include "client/ChatHandler.hpp"
#include "client/ChunkTransferHandler.hpp"
#include "client/PlayerJoinHandler.hpp"
#include "client/PlayerUpdateHandler.hpp"
#include "client/PlayerQuitHandler.hpp"

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

    case MessageType::ChunkTransfer:
      return Client::ChunkTransferHandler::handle(GS, msg);

    case MessageType::Chat:
      return Client::ChatHandler::handle(GS, msg);

    case MessageType::PlayerJoin:
      return Client::PlayerJoinHandler::handle(GS, msg);
    case MessageType::PlayerQuit:
      return Client::PlayerQuitHandler::handle(GS, msg);
    case MessageType::PlayerUpdate:
      return Client::PlayerUpdateHandler::handle(GS, msg);

    case MessageType::BlockUpdate:
      return Client::BlockUpdateHandler::handle(GS, msg);

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
