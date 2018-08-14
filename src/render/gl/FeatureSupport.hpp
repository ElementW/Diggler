#ifndef DIGGLER_RENDER_GL_FEATURE_SUPPORT_HPP
#define DIGGLER_RENDER_GL_FEATURE_SUPPORT_HPP

#include <string>

#include "../../platform/Types.hpp"

namespace diggler {
namespace render {
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
    buffer_storage,
    debug;

  static void probe();
  static std::string supported();
};

}
}
}

#endif /* DIGGLER_RENDER_GL_FEATURE_SUPPORT_HPP */
