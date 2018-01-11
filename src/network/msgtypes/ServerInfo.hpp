#ifndef DIGGLER_NET_MSGTYPES_SERVERINFO_HPP
#define DIGGLER_NET_MSGTYPES_SERVERINFO_HPP

#include "MsgType.hpp"

#include <meiose/variant.hpp>

namespace Diggler {
namespace Net {
namespace MsgTypes {

enum class ServerInfoSubtype : uint8 {
  Request = 0,
  Response = 1
};

struct ServerInfoRequest : public MsgType {
  std::vector<std::string> infos;
  meiose::variant params;

  void writeToMsg(OutMessage&) const override;
  void readFromMsg(InMessage&) override;
};

struct ServerInfoResponse : public MsgType {
  meiose::variant infos;

  void writeToMsg(OutMessage&) const override;
  void readFromMsg(InMessage&) override;
};

}
}
}

#endif /* DIGGLER_NET_MSGTYPES_SERVERINFO_HPP */
