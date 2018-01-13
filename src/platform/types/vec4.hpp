#ifndef DIGGLER_PLATFORM_TYPES_VEC4_HPP
#define DIGGLER_PLATFORM_TYPES_VEC4_HPP

#include <glm/vec4.hpp>

#include "../Types.hpp"

namespace diggler {

using vec4u8  = glm::tvec4<uint8>;
using vec4u16 = glm::tvec4<uint16>;
using vec4u32 = glm::tvec4<uint32>;
using vec4u64 = glm::tvec4<uint64>;
using vec4u   = vec4u64;
using vec4uvt = vec4u::value_type;

using vec4i8  = glm::tvec4<int8>;
using vec4i16 = glm::tvec4<int16>;
using vec4i32 = glm::tvec4<int32>;
using vec4i64 = glm::tvec4<int64>;
using vec4i   = vec4i64;
using vec4ivt = vec4i::value_type;

using vec4f   = glm::tvec4<float>;
using vec4d   = glm::tvec4<double>;
using vec4    = vec4d;
using vec4vt  = vec4::value_type;

}

#endif /* DIGGLER_PLATFORM_TYPES_VEC4_HPP */
