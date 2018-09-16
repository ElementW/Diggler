#ifndef DIGGLER_PLATFORM_TYPES_MAT3_HPP
#define DIGGLER_PLATFORM_TYPES_MAT3_HPP

#include <glm/mat3x3.hpp>

#include "../Types.hpp"

namespace diggler {

using mat3f = glm::tmat3x3<float>;
using mat3d = glm::tmat3x3<double>;
using mat3  = mat3f;

}

#endif /* DIGGLER_PLATFORM_TYPES_MAT3_HPP */
