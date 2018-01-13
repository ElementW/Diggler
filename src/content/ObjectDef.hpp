#ifndef DIGGLER_CONTENT_OBJECT_DEF_HPP
#define DIGGLER_CONTENT_OBJECT_DEF_HPP
#include "../Platform.hpp"

namespace diggler {
namespace content {

class ObjectDef {
public:
  enum class Variability : uint8 {
    Static,
    Dynamic,
    Stream
  };
};

}
}

#endif /* DIGGLER_CONTENT_OBJECT_DEF_HPP */
