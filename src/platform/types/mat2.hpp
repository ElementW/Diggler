#ifndef DIGGLER_PLATFORM_TYPES_MAT2_HPP
#define DIGGLER_PLATFORM_TYPES_MAT2_HPP

#include <glm/mat2x2.hpp>

#include "../Types.hpp"

namespace Diggler {

using mat2f   = glm::tmat2x2<float>;
using mat2d   = glm::tmat2x2<double>;
using mat2    = mat2d;

}

#endif /* DIGGLER_PLATFORM_TYPES_MAT2_HPP */
