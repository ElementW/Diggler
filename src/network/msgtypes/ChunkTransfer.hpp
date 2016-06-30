#ifndef DIGGLER_NET_MSGTYPES_CHUNKTRANSFER_HPP
#define DIGGLER_NET_MSGTYPES_CHUNKTRANSFER_HPP

#include "MsgType.hpp"

#include <glm/vec3.hpp>
#include <msgpack.hpp>

#include "../../Chunk.hpp"
#include "../../World.hpp"

namespace Diggler {
namespace Net {
namespace MsgTypes {

enum class ChunkTransferSubtype : uint8 {
  Request = 0,
  Response = 1,
  Denied
};

struct ChunkTransferRequest : public MsgType {
  struct ChunkData {
    WorldId worldId;
    glm::ivec3 chunkPos;
  };
  std::vector<ChunkData> chunks;

  void writeToMsg(OutMessage&) const override;
  void readFromMsg(InMessage&) override;
};

struct ChunkTransferResponse : public MsgType {
  struct ChunkData {
    WorldId worldId;
    glm::ivec3 chunkPos;
    uint32 dataLength;
    const void *data;
  };
  std::vector<ChunkData> chunks;

  void writeToMsg(OutMessage&) const override;
  void readFromMsg(InMessage&) override;
};

struct ChunkTransferDenied : public MsgType {
  enum Reason : uint8 {
    NotSpecified,
    PlayerTooFarAway,
    PlayerInWrongWorld,
    OutOfMapBounds
  } reason;

  void writeToMsg(OutMessage&) const override;
  void readFromMsg(InMessage&) override;
};

}
}
}

#endif /* DIGGLER_NET_MSGTYPES_CHUNKTRANSFER_HPP */
