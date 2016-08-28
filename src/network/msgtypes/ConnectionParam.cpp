#include "ConnectionParam.hpp"

namespace Diggler {
namespace Net {
namespace MsgTypes {

void ConnectionParamDHKeyExchange::writeToMsg(OutMessage &msg) const {
  msg.setType(MessageType::ConnectionParam, ConnectionParamSubtype::DHKeyExchange);

  msg.writeData(reinterpret_cast<const char*>(&pk.bytes), pk.Length);
}

void ConnectionParamDHKeyExchange::readFromMsg(InMessage &msg) {
  msg.readData(&pk.bytes, pk.Length);
}

}
}
}
