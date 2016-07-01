#ifndef DIGGLER_NET_MSGTYPES_MSGTYPE_HPP
#define DIGGLER_NET_MSGTYPES_MSGTYPE_HPP

#include "../Network.hpp"

namespace Diggler {
namespace Net {
namespace MsgTypes {

struct MsgType {
  virtual ~MsgType();

  virtual void writeToMsg(OutMessage&) const = 0;
  virtual void readFromMsg(InMessage&) = 0;
};

}
}
}

#endif /* DIGGLER_NET_MSGTYPES_MSGTYPE_HPP */
