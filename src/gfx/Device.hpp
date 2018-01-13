#ifndef DIGGLER_GFX_DEVICE_HPP
#define DIGGLER_GFX_DEVICE_HPP

#include "Command.hpp"

namespace diggler {
namespace gfx {

class Device {
public:
  virtual ~Device() = 0;

  virtual void queue(Command&&);
};

}
}

#endif /* DIGGLER_GFX_DEVICE_HPP */
