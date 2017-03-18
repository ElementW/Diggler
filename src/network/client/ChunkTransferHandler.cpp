#include "ChunkTransferHandler.hpp"

#include "../../Game.hpp"
#include "../../GameState.hpp"
#include "../msgtypes/ChunkTransfer.hpp"

namespace Diggler {
namespace Net {
namespace Client {

using namespace Net::MsgTypes;

bool ChunkTransferHandler::handle(GameState &GS, InMessage &msg) {
  using S = ChunkTransferSubtype;
  switch (msg.getSubtype<S>()) {
    case S::Request: {
      ; // No-op
    } break;
    case S::Response: {
      ChunkTransferResponse ctr;
      ctr.readFromMsg(msg);
      for (const ChunkTransferResponse::ChunkData &cd : ctr.chunks) {
        ChunkRef c = GS.G->U->getLoadWorld(cd.worldId)->getNewEmptyChunk(
          cd.chunkPos.x, cd.chunkPos.y, cd.chunkPos.z);
        IO::InMemoryStream ims(cd.data, cd.dataLength);
        c->read(ims);
        GS.holdChunksInMem.push_back(c);
      }
    } break;
    case S::Denied: {
      ChunkTransferDenied ctd;
      ctd.readFromMsg(msg);
    } break;
  }
  return true;
}

}
}
}
