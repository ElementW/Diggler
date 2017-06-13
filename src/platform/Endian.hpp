#ifndef DIGGLER_PLATFORM_ENDIAN_HPP
#define DIGGLER_PLATFORM_ENDIAN_HPP

#include <cstdint>

namespace Diggler {

constexpr uint16_t byteSwap16(uint16_t x) {
  return static_cast<uint16_t>((x >> 8) | (x << 8));
}

constexpr uint32_t byteSwap32(uint32_t x) {
  return (x>>24) | ((x<<8) & 0x00FF0000) | ((x>>8) & 0x0000FF00) | (x<<24);;
}

constexpr uint64_t byteSwap64(uint64_t x) {
  return (x>>56) | ((x<<40) & 0x00FF000000000000UL) | ((x<<24) & 0x0000FF0000000000UL) |
         ((x<<8)  & 0x000000FF00000000UL) | ((x>>8)  & 0x00000000FF000000UL) |
         ((x>>24) & 0x0000000000FF0000UL) | ((x>>40) & 0x000000000000FF00UL) | (x<<56);
}

enum class Endianness : uint8_t {
  Little,
  Big
};

#if defined(__BYTE_ORDER__) && __BYTE_ORDER__ ==  __ORDER_BIG_ENDIAN__ || \
    defined(__BYTE_ORDER) && __BYTE_ORDER == __BIG_ENDIAN || \
    defined(__BIG_ENDIAN__) || \
    defined(__ARMEB__) || \
    defined(__THUMBEB__) || \
    defined(__AARCH64EB__) || \
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
      defined(__ARMEL__) || \
      defined(__THUMBEL__) || \
      defined(__AARCH64EL__) || \
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
