#ifndef DIGGLER_NET_CLIENT_CONTENT_HANDLER_HPP
#define DIGGLER_NET_CLIENT_CONTENT_HANDLER_HPP

#include "Handler.hpp"

namespace diggler {
namespace net {
namespace Client {

class ContentHandler : public Handler {
public:
  static bool handle(Game&, InMessage&);
};

}
}
}

#endif /* DIGGLER_NET_CLIENT_CONTENT_HANDLER_HPP */
