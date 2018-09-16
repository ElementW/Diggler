#ifndef DIGGLER_PLATFORM_TYPES_VEC3_HPP
#define DIGGLER_PLATFORM_TYPES_VEC3_HPP

#include <glm/vec3.hpp>

#include "../Types.hpp"

namespace diggler {

using vec3u8  = glm::tvec3<uint8>;
using vec3u16 = glm::tvec3<uint16>;
using vec3u32 = glm::tvec3<uint32>;
using vec3u64 = glm::tvec3<uint64>;
using vec3u   = vec3u64;
using vec3uvt = vec3u::value_type;

using vec3i8  = glm::tvec3<int8>;
using vec3i16 = glm::tvec3<int16>;
using vec3i32 = glm::tvec3<int32>;
using vec3i64 = glm::tvec3<int64>;
using vec3i   = vec3i64;
using vec3ivt = vec3i::value_type;

using vec3f   = glm::tvec3<float>;
using vec3d   = glm::tvec3<double>;
using vec3    = vec3f;
using vec3vt  = vec3::value_type;

}

#endif /* DIGGLER_PLATFORM_TYPES_VEC3_HPP */
