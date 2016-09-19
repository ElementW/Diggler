#include "PlayerJoin.hpp"

namespace Diggler {
namespace Net {
namespace MsgTypes {

void PlayerJoinRequest::writeToMsg(OutMessage &msg) const {
  msg.setType(MessageType::PlayerJoin, PlayerJoinSubtype::Request);

  msg.writeString(name);
}

void PlayerJoinRequest::readFromMsg(InMessage &msg) {
  name = msg.readString();
}


void PlayerJoinSuccess::writeToMsg(OutMessage &msg) const {
  msg.setType(MessageType::PlayerJoin, PlayerJoinSubtype::Success);

  msg.writeU32(sessId);
  msg.writeI32(wid);
}

void PlayerJoinSuccess::readFromMsg(InMessage &msg) {
  sessId = msg.readU32();
  wid = msg.readI32();
}


void PlayerJoinFailure::writeToMsg(OutMessage &msg) const {
  msg.setType(MessageType::PlayerJoin, PlayerJoinSubtype::Failure);

  msg.writeU8(failureReason);
}

void PlayerJoinFailure::readFromMsg(InMessage &msg) {
  failureReason = static_cast<FailureReason>(msg.readU8());
}


void PlayerJoinBroadcast::writeToMsg(OutMessage &msg) const {
  msg.setType(MessageType::PlayerJoin, PlayerJoinSubtype::Broadcast);

  msg.writeU8(joinReason);
  msg.writeString(name);
  msg.writeU32(sessId);
  msg.writeI32(wid);
}

void PlayerJoinBroadcast::readFromMsg(InMessage &msg) {
  joinReason = static_cast<JoinReason>(msg.readU8());
  name = msg.readString();
  sessId = msg.readU32();
  wid = msg.readI32();
}

}
}
}
