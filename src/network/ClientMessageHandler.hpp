#ifndef DIGGLER_NET_CLIENT_MESSAGE_HANDLER_HPP
#define DIGGLER_NET_CLIENT_MESSAGE_HANDLER_HPP

#include "Network.hpp"

namespace Diggler {

class GameState;

namespace Net {

class ClientMessageHandler {
public:
  GameState &GS;

  ClientMessageHandler(GameState&);

  bool handleMessage(InMessage&);
};

}
}

#endif /* DIGGLER_NET_CLIENT_MESSAGE_HANDLER_HPP */
