#ifndef DIGGLER_NET_CLIENT_PLAYER_JOIN_HANDLER_HPP
#define DIGGLER_NET_CLIENT_PLAYER_JOIN_HANDLER_HPP

#include "Handler.hpp"

namespace diggler {
namespace net {
namespace Client {

class PlayerJoinHandler : public Handler {
public:
  static bool handle(Game&, InMessage&);
};

}
}
}

#endif /* DIGGLER_NET_CLIENT_PLAYER_JOIN_HANDLER_HPP */
