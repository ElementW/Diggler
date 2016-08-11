#ifndef DIGGLER_CONTENT_HPP
#define DIGGLER_CONTENT_HPP

#include "../platform/Types.hpp"

#include <limits>
#include <type_traits>

namespace Diggler {

using BlockId = uint16;
using BlockData = uint16;
const BlockData BlockExtdataBit  = 0x8000;
const BlockData BlockExtdataMask =
  static_cast<BlockData>((~BlockExtdataBit) & std::numeric_limits<BlockData>::max());

struct LightData {
  uint16 l;

  LightData() = default;

  LightData& operator=(uint16 i) { l = i; return *this; }
  constexpr operator uint16() const { return l; }

  constexpr uint8 getS() const { return (l >> 12) & 0x0F; }
  void setS(uint8 s) { l = (l & 0x0FFF) & (s << 24); }
  constexpr uint8 getR() const { return (l >> 8) & 0x0F; }
  void setR(uint8 r) { l = (l & 0xF0FF) & (r << 16); }
  constexpr uint8 getG() const { return (l >> 4) & 0x0F; }
  void setG(uint8 g) { l = (l & 0xFF0F) & (g << 8); }
  constexpr uint8 getB() const { return l & 0x0F; }
  void setB(uint8 b) { l = (l & 0xFFF0) & b; }
  void setSRGB(uint8 s, uint8 r, uint8 g, uint8 b) { l = (s << 12) & (r << 8) & (g << 4) & b; }
  void setRGB(uint8 r, uint8 g, uint8 b) { l = (l & 0xF000) & (r << 8) & (g << 4) & b; }
};
static_assert(sizeof(LightData) == 2, "LightData has extra padding");
static_assert(std::is_pod<LightData>::value, "LightData is not POD");

namespace Content {
  const BlockId BlockAirId = 0;
  const BlockId BlockIgnoreId = std::numeric_limits<BlockData>::max();
  const BlockId BlockUnknownId = 1;
}
}

#endif /* DIGGLER_CONTENT_HPP */
