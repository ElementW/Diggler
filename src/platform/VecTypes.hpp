#ifndef DIGGLER_PLATFORM_VEC_TYPES_HPP
#define DIGGLER_PLATFORM_VEC_TYPES_HPP

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "Types.hpp"

namespace Diggler {

using vec2u8  = glm::tvec2<uint8, glm::defaultp>;
using vec2u16 = glm::tvec2<uint16, glm::defaultp>;
using vec2u32 = glm::tvec2<uint32, glm::defaultp>;
using vec2u64 = glm::tvec2<uint64, glm::defaultp>;
using vec2u   = vec2u64;

using vec2i8  = glm::tvec2<int8, glm::defaultp>;
using vec2i16 = glm::tvec2<int16, glm::defaultp>;
using vec2i32 = glm::tvec2<int32, glm::defaultp>;
using vec2i64 = glm::tvec2<int64, glm::defaultp>;
using vec2i   = vec2i64;

using vec2f   = glm::tvec2<float, glm::defaultp>;
using vec2d   = glm::tvec2<double, glm::defaultp>;
using vec2    = vec2d;


using vec3u8  = glm::tvec3<uint8, glm::defaultp>;
using vec3u16 = glm::tvec3<uint16, glm::defaultp>;
using vec3u32 = glm::tvec3<uint32, glm::defaultp>;
using vec3u64 = glm::tvec3<uint64, glm::defaultp>;
using vec3u   = vec3u64;

using vec3i8  = glm::tvec3<int8, glm::defaultp>;
using vec3i16 = glm::tvec3<int16, glm::defaultp>;
using vec3i32 = glm::tvec3<int32, glm::defaultp>;
using vec3i64 = glm::tvec3<int64, glm::defaultp>;
using vec3i   = vec3i64;

using vec3f   = glm::tvec3<float, glm::defaultp>;
using vec3d   = glm::tvec3<double, glm::defaultp>;
using vec3    = vec3d;


using vec4u8  = glm::tvec4<uint8, glm::defaultp>;
using vec4u16 = glm::tvec4<uint16, glm::defaultp>;
using vec4u32 = glm::tvec4<uint32, glm::defaultp>;
using vec4u64 = glm::tvec4<uint64, glm::defaultp>;
using vec4u   = vec4u64;

using vec4i8  = glm::tvec4<int8, glm::defaultp>;
using vec4i16 = glm::tvec4<int16, glm::defaultp>;
using vec4i32 = glm::tvec4<int32, glm::defaultp>;
using vec4i64 = glm::tvec4<int64, glm::defaultp>;
using vec4i   = vec4i64;

using vec4f   = glm::tvec4<float, glm::defaultp>;
using vec4d   = glm::tvec4<double, glm::defaultp>;
using vec4    = vec4d;

}

#endif /* DIGGLER_PLATFORM_VEC_TYPES_HPP */
