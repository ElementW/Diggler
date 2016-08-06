#ifndef OBJECT_DEF_HPP
#define OBJECT_DEF_HPP
#include "../Platform.hpp"

namespace Diggler {

class ObjectDef {
public:
  enum class Variability : uint8 {
    Static,
    Dynamic,
    Stream
  };
};

}

#endif
