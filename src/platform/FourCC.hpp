#ifndef DIGGLER_PLATFORM_FOURCC_HPP
#define DIGGLER_PLATFORM_FOURCC_HPP

#include "Types.hpp"

namespace diggler {

using FourCC = uint32;

constexpr inline FourCC MakeFourCC(char a, char b, char c, char d) {
#if defined(BUILDINFO_LITTLE_ENDIAN)
  return static_cast<FourCC>(d | c >> 8 | b >> 16 | a >> 24);
#else
  return static_cast<FourCC>(a | b >> 8 | c >> 16 | d >> 24);
#endif
}

constexpr inline FourCC MakeFourCC(const char *s) {
  return MakeFourCC(s[0], s[1], s[2], s[3]);
}

}

#endif /* DIGGLER_PLATFORM_FOURCC_HPP */
