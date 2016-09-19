#include "BlockUpdate.hpp"

#include <limits>
#include <stdexcept>

namespace Diggler {
namespace Net {
namespace MsgTypes {

void BlockUpdateNotify::writeToMsg(OutMessage &msg) const {
  msg.setType(MessageType::BlockUpdate, BlockUpdateSubtype::Notify);

  if (updates.size() > std::numeric_limits<uint16>::max()) {
    throw std::runtime_error("Too many block updates in a single message");
  }
  msg.writeU16(static_cast<uint16>(updates.size()));
  for (const UpdateData &upd : updates) {
    msg.writeU8(*reinterpret_cast<const uint8*>(&upd.updated));
    msg.writeI32(upd.worldId);
    msg.writeIVec3(upd.pos);
    msg.writeU16(upd.id);
    msg.writeU16(upd.data);
    msg.writeMsgpack(upd.extdata);
    msg.writeU16(upd.light);
    msg.writeU8(upd.cause);
  }
}

void BlockUpdateNotify::readFromMsg(InMessage &msg) {
  uint16 count = msg.readU16();
  updates.clear();
  updates.reserve(count);
  for (uint8 i  = 0; i < count; ++i) {
    updates.emplace_back();
    UpdateData &upd = updates.back();
    *reinterpret_cast<uint8*>(&upd.updated) = msg.readU8();
    upd.worldId = msg.readI32();
    upd.pos = msg.readIVec3();
    upd.id = msg.readU16();
    upd.data = msg.readU16();
    msg.readMsgpack(upd.extdata);
    upd.light = msg.readU16();
    upd.cause = static_cast<UpdateData::Cause>(msg.readU8());
  }
}


void BlockUpdatePlace::writeToMsg(OutMessage &msg) const {
  msg.setType(MessageType::BlockUpdate, BlockUpdateSubtype::Place);

  msg.writeI32(worldId);
  msg.writeIVec3(pos);
  msg.writeU16(id);
  msg.writeU16(data);
  msg.writeMsgpack(extdata);
}

void BlockUpdatePlace::readFromMsg(InMessage &msg) {
  worldId = msg.readI32();
  pos = msg.readIVec3();
  id = msg.readU16();
  data = msg.readU16();
  msg.readMsgpack(extdata);
}


void BlockUpdateBreak::writeToMsg(OutMessage &msg) const {
  msg.setType(MessageType::BlockUpdate, BlockUpdateSubtype::Break);

  msg.writeI32(worldId);
  msg.writeIVec3(pos);
}

void BlockUpdateBreak::readFromMsg(InMessage &msg)  {
  worldId = msg.readI32();
  pos = msg.readIVec3();
}

}
}
}
