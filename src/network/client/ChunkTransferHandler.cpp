#include "ChunkTransferHandler.hpp"

#include "../../Game.hpp"
#include "../../states/GameState.hpp"
#include "../msgtypes/ChunkTransfer.hpp"

namespace diggler {
namespace net {
namespace Client {

using namespace net::MsgTypes;

bool ChunkTransferHandler::handle(Game &G, InMessage &msg) {
  using S = ChunkTransferSubtype;
  switch (msg.getSubtype<S>()) {
    case S::Request: {
      ; // No-op
    } break;
    case S::Response: {
      ChunkTransferResponse ctr;
      ctr.readFromMsg(msg);
      for (const ChunkTransferResponse::ChunkData &cd : ctr.chunks) {
        ChunkRef c = G.U->getLoadWorld(cd.worldId)->getNewEmptyChunk(
          cd.chunkPos.x, cd.chunkPos.y, cd.chunkPos.z);
        io::InMemoryStream ims(cd.data, cd.dataLength);
        c->read(ims);
        static_cast<states::GameState&>(G.GW->state()).holdChunksInMem.push_back(c);
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
