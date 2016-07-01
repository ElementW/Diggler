#include "ServerInfo.hpp"

namespace Diggler {
namespace Net {
namespace MsgTypes {

void ServerInfoRequest::readFromMsg(InMessage &msg) {
  uint8 infoCount = msg.readU8();
  infos.clear();
  for (int i = 0; i < infoCount; ++i) {
    infos.emplace_back(msg.readString());
  }
  Stream::PosT offset = msg.tell();
  params = msgpack::unpack(z, static_cast<const char*>(msg.data()), msg.length(), offset);
}


void ServerInfoRequest::writeToMsg(OutMessage &msg) const {
  msg.setType(MessageType::ServerInfo, 0);

  msg.writeU8(infos.size());
  for (const std::string &s : infos) {
    msg.writeString(s);
  }
  msgpack::pack(msg, params);
}

}
}
}
