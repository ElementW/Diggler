#ifndef DIGGLER_PIXEL_FORMAT_HPP
#define DIGGLER_PIXEL_FORMAT_HPP

#include "platform/Types.hpp"

namespace Diggler {

enum class PixelFormat : uint8 {
  RGB,
  RGBA
};

constexpr uint PixelFormatByteSize(PixelFormat pf) {
  switch (pf) {
  case PixelFormat::RGB:
    return 3;
  case PixelFormat::RGBA:
    return 4;
  }
  return 0;
}

}

#endif /* DIGGLER_PIXEL_FORMAT_HPP */
