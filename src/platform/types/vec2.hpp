#ifndef DIGGLER_PLATFORM_TYPES_VEC2_HPP
#define DIGGLER_PLATFORM_TYPES_VEC2_HPP

#include <glm/vec2.hpp>

#include "../Types.hpp"

namespace diggler {

using vec2u8  = glm::tvec2<uint8>;
using vec2u16 = glm::tvec2<uint16>;
using vec2u32 = glm::tvec2<uint32>;
using vec2u64 = glm::tvec2<uint64>;
using vec2u   = vec2u64;
using vec2uvt = vec2u::value_type;

using vec2i8  = glm::tvec2<int8>;
using vec2i16 = glm::tvec2<int16>;
using vec2i32 = glm::tvec2<int32>;
using vec2i64 = glm::tvec2<int64>;
using vec2i   = vec2i64;
using vec2ivt = vec2i::value_type;

using vec2f   = glm::tvec2<float>;
using vec2d   = glm::tvec2<double>;
using vec2    = vec2f;
using vec2vt  = vec2::value_type;

}

#endif /* DIGGLER_PLATFORM_TYPES_VEC2_HPP */
