#ifndef DIGGLER_UTIL_COLOR_UTIL_HPP
#define DIGGLER_UTIL_COLOR_UTIL_HPP

#include "../platform/Types.hpp"

namespace Diggler {
namespace Util {
namespace ColorUtil {

void fillRGB888(void *dst, byte r, byte g, byte b, uint numPixels);
void fillRGBA8888(void *dst, byte r, byte g, byte b, byte a, uint numPixels);

}
}
}

#endif /* DIGGLER_UTIL_COLOR_UTIL_HPP */
