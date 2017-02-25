#ifndef DIGGLER_NET_CLIENT_BLOCK_UPDATE_HANDLER_HPP
#define DIGGLER_NET_CLIENT_BLOCK_UPDATE_HANDLER_HPP

#include "Handler.hpp"

namespace Diggler {
namespace Net {
namespace Client {

class BlockUpdateHandler : public Handler {
public:
  static bool handle(GameState&, InMessage&);
};

}
}
}

#endif /* DIGGLER_NET_CLIENT_BLOCK_UPDATE_HANDLER_HPP */
