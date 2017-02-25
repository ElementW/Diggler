#ifndef DIGGLER_NET_CLIENT_CHUNK_TRANSFER_HANDLER_HPP
#define DIGGLER_NET_CLIENT_CHUNK_TRANSFER_HANDLER_HPP

#include "Handler.hpp"

namespace Diggler {
namespace Net {
namespace Client {

class ChunkTransferHandler : public Handler {
public:
  static bool handle(GameState&, InMessage&);
};

}
}
}

#endif /* DIGGLER_NET_CLIENT_CHUNK_TRANSFER_HANDLER_HPP */
