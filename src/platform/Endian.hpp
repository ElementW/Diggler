#ifndef DIGGLER_PLATFORM_ENDIAN_HPP
#define DIGGLER_PLATFORM_ENDIAN_HPP

#include <cstdint>

namespace Diggler {

constexpr uint16_t byteSwap16(uint16_t x) {
#if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
  return __builtin_bswap16(x);
#else
  return static_cast<uint16_t>((x >> 8) | (x << 8));
#endif
}

constexpr uint32_t byteSwap32(uint32_t x) {
#if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
  return __builtin_bswap32(x);
#elif defined(__ICC) || defined(__INTEL_COMPILER)
  return _bswap(x);
#else
  return (x>>24) | ((x<<8) & 0x00FF0000) | ((x>>8) & 0x0000FF00) | (x<<24);
#endif
}

constexpr uint64_t byteSwap64(uint64_t x) {
#if defined(__clang__) || defined(__GNUC__) || defined(__GNUG__)
  return __builtin_bswap64(x);
#elif defined(__ICC) || defined(__INTEL_COMPILER)
  return _bswap64(x);
#else
  return (x>>56) | ((x<<40) & 0x00FF000000000000UL) | ((x<<24) & 0x0000FF0000000000UL) |
         ((x<<8)  & 0x000000FF00000000UL) | ((x>>8)  & 0x00000000FF000000UL) |
         ((x>>24) & 0x0000000000FF0000UL) | ((x>>40) & 0x000000000000FF00UL) | (x<<56);
#endif
}

enum class Endianness : uint8_t {
  Little,
  Big
};

#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ ==  __ORDER_BIG_ENDIAN__ || \
    defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN || \
    defined(__BIG_ENDIAN__) || \
    defined(__sparc) || defined(__sparc__) || \
    defined(_POWER) || defined(__powerpc__) || \
    defined(__ppc__) || defined(__hpux) || \
    defined(__s390__) || \
    defined(__ARMEB__) || defined(__THUMBEB__) || defined(__AARCH64EB__) || \
    defined(_MIPSEB) || defined(__MIPSEB) || defined(__MIPSEB__)

constexpr Endianness SystemEndianness = Endianness::Big;

constexpr uint16_t toBe16(uint16_t x) { return x; }
constexpr uint16_t toLe16(uint16_t x) { return byteSwap16(x); }
constexpr uint16_t fromBe16(uint16_t x) { return x; }
constexpr uint16_t fromLe16(uint16_t x) { return byteSwap16(x); }

constexpr uint32_t toBe32(uint32_t x) { return x; }
constexpr uint32_t toLe32(uint32_t x) { return byteSwap32(x); }
constexpr uint32_t fromBe32(uint32_t x) { return x; }
constexpr uint32_t fromLe32(uint32_t x) { return byteSwap32(x); }

constexpr uint64_t toBe64(uint64_t x) { return x; }
constexpr uint64_t toLe64(uint64_t x) { return byteSwap64(x); }
constexpr uint64_t fromBe64(uint64_t x) { return x; }
constexpr uint64_t fromLe64(uint64_t x) { return byteSwap64(x); }
#elif defined(__BYTE_ORDER__) && __BYTE_ORDER__ ==  __ORDER_LITTLE_ENDIAN__ || \
      defined(__BYTE_ORDER) && __BYTE_ORDER == __LITTLE_ENDIAN || \
      defined(__LITTLE_ENDIAN__) || \
      defined(__i386__) || defined(__alpha__) || \
      defined(__ia64) || defined(__ia64__) || \
      defined(_M_IX86) || defined(_M_IA64) || \
      defined(_M_ALPHA) || defined(__amd64) || \
      defined(__amd64__) || defined(_M_AMD64) || \
      defined(__x86_64) || defined(__x86_64__) || defined(_M_X64) || \
      defined(__ARMEL__) || defined(__THUMBEL__) || defined(__AARCH64EL__) || \
      defined(_MIPSEL) || defined(__MIPSEL) || defined(__MIPSEL__)

constexpr Endianness SystemEndianness = Endianness::Little;

constexpr uint16_t toBe16(uint16_t x) { return byteSwap16(x); }
constexpr uint16_t toLe16(uint16_t x) { return x; }
constexpr uint16_t fromBe16(uint16_t x) { return byteSwap16(x); }
constexpr uint16_t fromLe16(uint16_t x) { return x; }

constexpr uint32_t toBe32(uint32_t x) { return byteSwap32(x); }
constexpr uint32_t toLe32(uint32_t x) { return x; }
constexpr uint32_t fromBe32(uint32_t x) { return byteSwap32(x); }
constexpr uint32_t fromLe32(uint32_t x) { return x; }

constexpr uint64_t toBe64(uint64_t x) { return byteSwap64(x); }
constexpr uint64_t toLe64(uint64_t x) { return x; }
constexpr uint64_t fromBe64(uint64_t x) { return byteSwap64(x); }
constexpr uint64_t fromLe64(uint64_t x) { return x; }
#else
#error "Unable to determine endianness"
#endif

}

#endif /* DIGGLER_PLATFORM_ENDIAN_HPP */
