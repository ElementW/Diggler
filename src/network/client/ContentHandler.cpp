#include "ContentHandler.hpp"

#include "../msgtypes/Content.hpp"

namespace diggler {
namespace net {
namespace Client {

using namespace net::MsgTypes;

bool ContentHandler::handle(Game &G, InMessage &msg) {
  using S = ContentSubtype;
  switch (msg.getSubtype<S>()) {
  case S::AssetResponse: {
    // TODO
  } break;
  case S::FileTransfer: {
    // No-op
  } break;
  case S::ModInfoResponse: {
    // No-op
  } break;
  case S::ModListResponse: {
    // No-op
  } break;

  case S::AssetRequest:
  case S::FileRequest:
  case S::ModInfoRequest:
  case S::ModListRequest: {
    // No-op
  } break;
  }
  return true;
}

}
}
}
