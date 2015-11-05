#ifndef NET_MSGTYPES_SERVERINFO_HPP
#define NET_MSGTYPES_SERVERINFO_HPP
#include "../Network.hpp"

namespace Diggler {
namespace Net {
namespace MsgTypes {

enum class ServerInfoSubtype : uint8 {
	Request = 0,
	Response = 1
};

struct ServerInfoRequest {
	std::vector<std::string> infos;

	void writeToMsg(OutMessage&) const;
	void readFromMsg(InMessage&);
};

struct ServerInfoResponse {
  /*TODO*/;
};

}
}
}

#endif