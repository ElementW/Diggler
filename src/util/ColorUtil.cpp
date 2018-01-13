#include "ColorUtil.hpp"

#include "Memset.hpp"

namespace diggler {
namespace Util {
namespace ColorUtil {

void fillRGB888(void *dst, byte r, byte g, byte b, uint numPixels) {
  alignas(alignof(uint32)) const byte color[3] = { r, g, b };
  memset24(dst, color, numPixels);
}

void fillRGBA8888(void *dst, byte r, byte g, byte b, byte a, uint numPixels) {
  alignas(alignof(uint32)) const byte color[4] = { r, g, b, a };
  // TODO: check if this works on all platforms; alignment issues might arise
  memset32(dst, *reinterpret_cast<const uint32*>(color), numPixels);
}

}
}
}
