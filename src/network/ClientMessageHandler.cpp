#include "ClientMessageHandler.hpp"

#include "../states/GameState.hpp"
#include "client/BlockUpdateHandler.hpp"
#include "client/ContentHandler.hpp"
#include "client/ChatHandler.hpp"
#include "client/ChunkTransferHandler.hpp"
#include "client/PlayerJoinHandler.hpp"
#include "client/PlayerUpdateHandler.hpp"
#include "client/PlayerQuitHandler.hpp"

namespace diggler {
namespace net {

ClientMessageHandler::ClientMessageHandler(states::GameState &gameState) :
  GS(gameState) {
}

bool ClientMessageHandler::handleMessage(InMessage &msg) {
  using namespace net::MsgTypes;
  Game &G = *GS.G;
  switch (msg.getType()) {
    case MessageType::NetDisconnect:
      GS.GW->showMessage("Disconnected", "Timed out");
      return false;

    case MessageType::ChunkTransfer:
      return Client::ChunkTransferHandler::handle(G, msg);

    case MessageType::Chat:
      return Client::ChatHandler::handle(G, msg);

    case MessageType::PlayerJoin:
      return Client::PlayerJoinHandler::handle(G, msg);
    case MessageType::PlayerQuit:
      return Client::PlayerQuitHandler::handle(G, msg);
    case MessageType::PlayerUpdate:
      return Client::PlayerUpdateHandler::handle(G, msg);

    case MessageType::BlockUpdate:
      return Client::BlockUpdateHandler::handle(G, msg);

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
