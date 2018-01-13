#include "BlockUpdateHandler.hpp"

#include "../../Game.hpp"
#include "../msgtypes/BlockUpdate.hpp"

namespace diggler {
namespace net {
namespace Client {

using namespace net::MsgTypes;

bool BlockUpdateHandler::handle(Game &G, InMessage &msg) {
  // TODO handle that in Chunk's ChangeHelper
  using S = BlockUpdateSubtype;
  constexpr auto CX = Chunk::CX, CY = Chunk::CY, CZ = Chunk::CZ;
  switch (msg.getSubtype<S>()) {
    case S::Notify: {
      BlockUpdateNotify bun;
      bun.readFromMsg(msg);
      for (const BlockUpdateNotify::UpdateData &upd : bun.updates) {
        WorldRef w = G.U->getWorld(upd.worldId);
        if (w) {
          ChunkRef c = w->getChunkAtCoords(upd.pos);
          if (c) {
            c->setBlock(rmod(upd.pos.x, CX), rmod(upd.pos.y, CY), rmod(upd.pos.z, CZ),
                        upd.id, upd.data);
            // TODO extdata & light
          }
        }
      }
    } break;
    case S::Place:
    case S::Break: {
      ; // No-op
    } break;
  }
  return true;
}

}
}
}
