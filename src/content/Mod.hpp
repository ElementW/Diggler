#ifndef DIGGLER_CONTENT_MOD_HPP
#define DIGGLER_CONTENT_MOD_HPP

#include <string>

#include "../Platform.hpp"

namespace Diggler {
namespace Content {

using ModId = byte[32];

class Mod {
public:
  ModId id;
  std::string name, fsPath;
};

}
}

#endif /* DIGGLER_CONTENT_MOD_HPP */
