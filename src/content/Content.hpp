#ifndef CONTENT_HPP
#define CONTENT_HPP
#include "../Platform.hpp"

namespace Diggler {

typedef uint16 BlockId;
typedef uint16 BlockData;
const uint16 BlockMetadataBit  = 0x8000;
const uint16 BlockMetadataMask = ~BlockMetadataMask;
typedef uint16 LightData;

namespace Content {
	const BlockId BlockAirId = 0;
	const BlockId BlockIgnoreId = ~BlockAirId;
	const BlockId BlockDefaultId = 1;
}
}

#endif