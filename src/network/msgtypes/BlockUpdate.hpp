#ifndef DIGGLER_NET_MSGTYPES_BLOCK_UPDATE_HPP
#define DIGGLER_NET_MSGTYPES_BLOCK_UPDATE_HPP

#include "MsgType.hpp"

#include "../../content/Content.hpp"
#include "../../World.hpp"

namespace Diggler {
namespace Net {
namespace MsgTypes {

enum class BlockUpdateSubtype : uint8 {
  Notify = 0,
  Place,
  Break
};

struct BlockUpdateNotify : public MsgType {
  struct UpdateData {
    struct Updated {
      bool : 4, light : 1, extdata : 1, data : 1, id : 1;
    } updated;
    static_assert(sizeof(updated) == 1, "Bitfields aren't packed");
    WorldId worldId;
    glm::ivec3 pos;
    BlockId id;
    BlockData data;
    //goodform::variant extdata;
    LightData light;
    enum Cause : uint8 {
      Unspecified = 0,
      PlayerPlace,
      PlayerBreak
    } cause;
  };
  std::vector<UpdateData> updates;

  void writeToMsg(OutMessage&) const override;
  void readFromMsg(InMessage&) override;
};

struct BlockUpdatePlace : public MsgType {
  WorldId worldId;
  glm::ivec3 pos;
  BlockId id;
  BlockData data;
  //goodform::variant extdata;

  void writeToMsg(OutMessage&) const override;
  void readFromMsg(InMessage&) override;
};

struct BlockUpdateBreak : public MsgType {
  WorldId worldId;
  glm::ivec3 pos;

  void writeToMsg(OutMessage&) const override;
  void readFromMsg(InMessage&) override;
};

}
}
}

#endif /* DIGGLER_NET_MSGTYPES_BLOCK_UPDATE_HPP */
