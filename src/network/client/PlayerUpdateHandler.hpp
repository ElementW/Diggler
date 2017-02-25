#ifndef DIGGLER_NET_CLIENT_PLAYER_UPDATE_HANDLER_HPP
#define DIGGLER_NET_CLIENT_PLAYER_UPDATE_HANDLER_HPP

#include "Handler.hpp"

namespace Diggler {
namespace Net {
namespace Client {

class PlayerUpdateHandler : public Handler {
public:
  static bool handle(GameState&, InMessage&);
};

}
}
}

#endif /* DIGGLER_NET_CLIENT_PLAYER_UPDATE_HANDLER_HPP */
