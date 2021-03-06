#ifndef DIGGLER_PLATFORM_TYPES_MAT2_HPP
#define DIGGLER_PLATFORM_TYPES_MAT2_HPP

#include <glm/mat2x2.hpp>

#include "../Types.hpp"

namespace diggler {

using mat2f = glm::tmat2x2<float>;
using mat2d = glm::tmat2x2<double>;
using mat2  = mat2f;

}

#endif /* DIGGLER_PLATFORM_TYPES_MAT2_HPP */
