#include "ContentHandler.hpp"

#include "../../GameState.hpp"
#include "../msgtypes/Content.hpp"

namespace Diggler {
namespace Net {
namespace Client {

using namespace Net::MsgTypes;

bool ContentHandler::handle(GameState &GS, InMessage &msg) {
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
