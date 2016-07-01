#include "Chat.hpp"

namespace Diggler {
namespace Net {
namespace MsgTypes {

void ChatSend::writeToMsg(OutMessage &omsg) const {
  omsg.setType(MessageType::Chat, ChatSubtype::Send);

  msgpack::pack(omsg, msg);
}

void ChatSend::readFromMsg(InMessage &imsg) {
  msg = msgpack::unpack(z, static_cast<const char*>(imsg.data()), imsg.length());
}


void ChatAnnouncement::writeToMsg(OutMessage &omsg) const {
  omsg.setType(MessageType::Chat, ChatSubtype::Announcement);

  omsg.writeString(announcementType);
  omsg.writeString(origin.name);
  msgpack::pack(omsg, origin.display);
  msgpack::pack(omsg, msg);
}

void ChatAnnouncement::readFromMsg(InMessage &imsg) {
  announcementType = imsg.readString();
  origin.name = imsg.readString();
  Stream::PosT offset = imsg.tell();
  origin.display = msgpack::unpack(static_cast<const char*>(imsg.data()),
    imsg.length(), offset).get();
  msg = msgpack::unpack(z, static_cast<const char*>(imsg.data()), imsg.length(), offset);
}


void ChatPlayerTalk::writeToMsg(OutMessage &omsg) const {
  omsg.setType(MessageType::Chat, ChatSubtype::PlayerTalk);

  omsg.writeU32(player.id);
  msgpack::pack(omsg, player.display);
  msgpack::pack(omsg, msg);
}

void ChatPlayerTalk::readFromMsg(InMessage &imsg) {
  player.id = imsg.readU32();
  Stream::PosT offset = imsg.tell();
  player.display = msgpack::unpack(static_cast<const char*>(imsg.data()),
    imsg.length(), offset).get();
  msg = msgpack::unpack(z, static_cast<const char*>(imsg.data()), imsg.length(), offset);
}

}
}
}
