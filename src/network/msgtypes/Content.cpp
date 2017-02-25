#include "Content.hpp"

namespace Diggler {
namespace Net {
namespace MsgTypes {

void ContentModListRequest::writeToMsg(OutMessage&) const {

}

void ContentModListRequest::readFromMsg(InMessage&) {

}


void ContentModListResponse::writeToMsg(OutMessage &msg) const {
  msg.writeI32(modsIds.size());
  for (const Content::ModId &modId : modsIds) {
    msg.writeData(&modId, sizeof(modId));
  }
}

void ContentModListResponse::readFromMsg(InMessage &msg) {
  modsIds.resize(msg.readI32());
  for (decltype(modsIds)::size_type i = 0; i < modsIds.size(); ++i) {
    msg.readData(&modsIds[i], sizeof(decltype(modsIds)::value_type));
  }
}

}
}
}
