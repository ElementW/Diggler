#include "BlockUpdateHandler.hpp"

#include "../../Game.hpp"
#include "../../GameState.hpp"
#include "../msgtypes/BlockUpdate.hpp"

namespace Diggler {
namespace Net {
namespace Client {

using namespace Net::MsgTypes;

bool BlockUpdateHandler::handle(GameState &GS, InMessage &msg) {
  // TODO handle that in Chunk's ChangeHelper
  using S = BlockUpdateSubtype;
  switch (msg.getSubtype<S>()) {
    case S::Notify: {
      BlockUpdateNotify bun;
      bun.readFromMsg(msg);
      for (const BlockUpdateNotify::UpdateData &upd : bun.updates) {
        WorldRef w = GS.G->U->getWorld(upd.worldId);
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
