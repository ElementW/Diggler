#ifndef NET_MSGTYPES_MSGTYPE_HPP
#define NET_MSGTYPES_MSGTYPE_HPP
#include "../Network.hpp"

namespace Diggler {
namespace Net {
namespace MsgTypes {

struct MsgType {
	virtual void writeToMsg(OutMessage&) const = 0;
	virtual void readFromMsg(InMessage&) = 0;
};

}
}
}

#endif
