#ifndef DIGGLER_CONTENT_MOD_HPP
#define DIGGLER_CONTENT_MOD_HPP

#include <string>

#include "../Platform.hpp"
#include "../crypto/Sign.hpp"

namespace diggler {
namespace content {

union ModId {
  struct Parts {
    byte uniqueId[32]; // 32 bytes
    Crypto::Sign::PublicKey creatorPk; // 32 bytes
    Crypto::Sign::Signature signature; // 64 bytes
  } parts;
  byte bytes[128];
};

class Mod {
public:
  ModId id;
  std::string name, fsPath;
};

}
}

#endif /* DIGGLER_CONTENT_MOD_HPP */
