#ifndef DIGGLER_RENDER_GL_FEATURE_SUPPORT_HPP
#define DIGGLER_RENDER_GL_FEATURE_SUPPORT_HPP

#include "../../Platform.hpp"

#include <epoxy/gl.h>

namespace Diggler {
namespace Render {
namespace gl {

class FeatureSupport {
public:
  FeatureSupport() = delete;

  static bool
    VAO;

  static void probe();
};

}
}
}

#endif /* DIGGLER_RENDER_GL_FEATURE_SUPPORT_HPP */
