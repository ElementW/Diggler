#ifndef DIGGLER_NET_CLIENT_BLOCK_UPDATE_HANDLER_HPP
#define DIGGLER_NET_CLIENT_BLOCK_UPDATE_HANDLER_HPP

#include "Handler.hpp"

namespace diggler {
namespace net {
namespace Client {

class BlockUpdateHandler : public Handler {
public:
  static bool handle(Game&, InMessage&);
};

}
}
}

#endif /* DIGGLER_NET_CLIENT_BLOCK_UPDATE_HANDLER_HPP */
