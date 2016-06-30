#ifndef CONTENT_HPP
#define CONTENT_HPP
#include "../Platform.hpp"

namespace Diggler {

using BlockId = uint16;
using BlockData = uint16;
const uint16 BlockMetadataBit  = 0x8000;
const uint16 BlockMetadataMask = 0x7fff; //~BlockMetadataBit;
using LightData = uint16;

namespace Content {
  const BlockId BlockAirId = 0;
  const BlockId BlockIgnoreId = ~BlockAirId;
  const BlockId BlockUnknownId = 1;
}
}

#endif
