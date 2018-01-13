#ifndef DIGGLER_UTIL_MEMSET_HPP
#define DIGGLER_UTIL_MEMSET_HPP

#include <cstring>

#include "../platform/Types.hpp"

namespace diggler {
namespace Util {

void memset8(void *dst, byte val, uint num) {
  memset(dst, val, num);
}

void memset16(void *dst, const uint16 val, uint num) {
  uint16 *u16dst = reinterpret_cast<uint16*>(dst);
  for (uint i = 0; i < num; ++i) {
    *u16dst++ = val;
  }
}

void memset32(void *dst, const uint32 val, uint num) {
  uint32 *u32dst = reinterpret_cast<uint32*>(dst);
  for (uint i = 0; i < num; ++i) {
    *u32dst++ = val;
  }
}

void memset24(void *dst, const void *src, uint num) {
  uint8 *u8dst = reinterpret_cast<uint8*>(dst);
  for (uint i = 0; i < num; ++i) {
    memcpy(u8dst + i * 3, src, 3);
  }
}

void memsetSized(byte *dst, const byte *src, uint num, uint stepsize) {
  uint8 *u8dst = reinterpret_cast<uint8*>(dst);
  for (uint i = 0; i < num; ++i) {
    memcpy(u8dst + i * stepsize, src, stepsize);
  }
}

}
}

#endif /* DIGGLER_UTIL_MEMSET_HPP */
