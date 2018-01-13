#ifndef DIGGLER_NET_MSGTYPES_CONNECTIONPARAM_HPP
#define DIGGLER_NET_MSGTYPES_CONNECTIONPARAM_HPP

#include "MsgType.hpp"

#include "../../crypto/DiffieHellman.hpp"

namespace diggler {
namespace net {
namespace MsgTypes {

enum class ConnectionParamSubtype : uint8 {
  DHKeyExchange
};

struct ConnectionParamDHKeyExchange : public MsgType {
  Crypto::DiffieHellman::PublicKey pk;

  void writeToMsg(OutMessage&) const override;
  void readFromMsg(InMessage&) override;
};

}
}
}

#endif /* DIGGLER_NET_MSGTYPES_CONNECTIONPARAM_HPP */
