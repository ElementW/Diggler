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
    VAO /* Vertex Array Objects */,
    DSA_ARB /* Direct State Access, ARB version */,
    DSA_EXT /* Direct State Access, EXT version */,
    DSA /* Direct State Access, one or move of above */,
    shader_image_load_store,
    FBO_ARB /* FrameBuffer Obects, ARB version */,
    buffer_storage;

  static void probe();
  static std::string supported();
};

}
}
}

#endif /* DIGGLER_RENDER_GL_FEATURE_SUPPORT_HPP */
