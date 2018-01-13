#include "ServerInfo.hpp"

#include <limits>
#include <stdexcept>

namespace diggler {
namespace net {
namespace MsgTypes {

void ServerInfoRequest::readFromMsg(InMessage &msg) {
  uint16 infoCount = msg.readU16();
  infos.clear();
  for (int i = 0; i < infoCount; ++i) {
    infos.emplace_back(msg.readString());
  }
  msg.readMsgpack(params);
}


void ServerInfoRequest::writeToMsg(OutMessage &msg) const {
  msg.setType(MessageType::ServerInfo, 0);

  if (infos.size() > std::numeric_limits<uint16>::max()) {
    throw std::runtime_error("Too many server info requests a single message");
  }
  msg.writeU16(static_cast<uint16>(infos.size()));
  for (const std::string &s : infos) {
    msg.writeString(s);
  }
  msg.writeMsgpack(params);
}

}
}
}
