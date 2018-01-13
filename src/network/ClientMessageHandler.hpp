#ifndef DIGGLER_NET_CLIENT_MESSAGE_HANDLER_HPP
#define DIGGLER_NET_CLIENT_MESSAGE_HANDLER_HPP

#include "Network.hpp"

namespace diggler {

namespace states {
class GameState;
}

namespace net {

class ClientMessageHandler {
public:
  states::GameState &GS;

  ClientMessageHandler(states::GameState&);

  bool handleMessage(InMessage&);
};

}
}

#endif /* DIGGLER_NET_CLIENT_MESSAGE_HANDLER_HPP */
