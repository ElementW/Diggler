#include "PlayerUpdate.hpp"

namespace Diggler {
namespace Net {
namespace MsgTypes {

enum FieldFlags {
  PlayerSessID = 0x1,
  Position = 0x2,
  Velocity = 0x4,
  Accel = 0x8
};

void PlayerUpdateMove::writeToMsg(OutMessage &msg) const {
  msg.setType(MessageType::PlayerUpdate, PlayerUpdateSubtype::Move);


  FieldFlags fields = static_cast<FieldFlags>(
    (plrSessId ? FieldFlags::PlayerSessID : 0) |
    (position ? FieldFlags::Position : 0) |
    (velocity ? FieldFlags::Velocity : 0) |
    (accel ? FieldFlags::Accel : 0));
  msg.writeU8(fields);
  if (plrSessId) {
    msg.writeU32(*plrSessId);
  }
  if (position) {
    msg.writeVec3(*position);
  }
  if (velocity) {
    msg.writeVec3(*velocity);
  }
  if (accel) {
    msg.writeVec3(*accel);
  }
  msg.writeFloat(angle);
}

void PlayerUpdateMove::readFromMsg(InMessage &msg) {

  FieldFlags fields = static_cast<FieldFlags>(msg.readU8());
  if (fields & FieldFlags::PlayerSessID) {
    plrSessId = msg.readU32();
  }
  if (fields & FieldFlags::Position) {
    position = msg.readVec3();
  }
  if (fields & FieldFlags::Velocity) {
    velocity = msg.readVec3();
  }
  if (fields & FieldFlags::Accel) {
    accel = msg.readVec3();
  }
  angle = msg.readFloat();
}


void PlayerUpdateDie::writeToMsg(OutMessage &msg) const {
  msg.setType(MessageType::PlayerUpdate, PlayerUpdateSubtype::Die);

  msg.writeU32(plrSessId);
  msg.writeU8(static_cast<uint8>(reason));
}

void PlayerUpdateDie::readFromMsg(InMessage &msg) {
  plrSessId = msg.readU32();
  reason = static_cast<Player::DeathReason>(msg.readU8());
}


void PlayerUpdateRespawn::writeToMsg(OutMessage &msg) const {
  msg.setType(MessageType::PlayerUpdate, PlayerUpdateSubtype::Respawn);

  msg.writeU32(plrSessId);
}

void PlayerUpdateRespawn::readFromMsg(InMessage &msg) {
  plrSessId = msg.readU32();
}

}
}
}
