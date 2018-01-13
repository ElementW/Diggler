#ifndef DIGGLER_PLATFORM_TYPES_MAT4_HPP
#define DIGGLER_PLATFORM_TYPES_MAT4_HPP

#include <glm/mat4x4.hpp>

#include "../Types.hpp"

namespace diggler {

using mat4f   = glm::tmat4x4<float>;
using mat4d   = glm::tmat4x4<double>;
using mat4    = mat4d;

}

#endif /* DIGGLER_PLATFORM_TYPES_MAT4_HPP */
