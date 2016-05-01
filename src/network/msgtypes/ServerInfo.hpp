#ifndef NET_MSGTYPES_SERVERINFO_HPP
#define NET_MSGTYPES_SERVERINFO_HPP
#include <msgpack.hpp>
#include "../Network.hpp"
#include "MsgType.hpp"

namespace Diggler {
namespace Net {
namespace MsgTypes {

enum class ServerInfoSubtype : uint8 {
	Request = 0,
	Response = 1
};

struct ServerInfoRequest : public MsgType {
	std::vector<std::string> infos;
	msgpack::object params;

	void writeToMsg(OutMessage&) const;
	void readFromMsg(InMessage&);
};

struct ServerInfoResponse : public MsgType {
	msgpack::object infos;

	void writeToMsg(OutMessage&) const;
	void readFromMsg(InMessage&);
};

}
}
}

#endif
