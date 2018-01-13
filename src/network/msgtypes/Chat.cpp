#include "Chat.hpp"

namespace diggler {
namespace net {
namespace MsgTypes {

void ChatSend::writeToMsg(OutMessage &omsg) const {
  omsg.setType(MessageType::Chat, ChatSubtype::Send);

  omsg.writeMsgpack(msg);
}

void ChatSend::readFromMsg(InMessage &imsg) {
  imsg.readMsgpack(msg);
}


void ChatAnnouncement::writeToMsg(OutMessage &omsg) const {
  omsg.setType(MessageType::Chat, ChatSubtype::Announcement);

  omsg.writeString(announcementType);
  omsg.writeString(origin.name);
  omsg.writeMsgpack(origin.display);
  omsg.writeMsgpack(msg);
}

void ChatAnnouncement::readFromMsg(InMessage &imsg) {
  announcementType = imsg.readString();
  origin.name = imsg.readString();
  imsg.readMsgpack(origin.display);
  imsg.readMsgpack(msg);
}


void ChatPlayerTalk::writeToMsg(OutMessage &omsg) const {
  omsg.setType(MessageType::Chat, ChatSubtype::PlayerTalk);

  omsg.writeU32(player.id);
  omsg.writeMsgpack(player.display);
  omsg.writeMsgpack(msg);
}

void ChatPlayerTalk::readFromMsg(InMessage &imsg) {
  player.id = imsg.readU32();
  imsg.readMsgpack(player.display);
  imsg.readMsgpack(msg);
}

}
}
}
