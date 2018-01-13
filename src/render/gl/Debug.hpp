#ifndef DIGGLER_RENDER_GL_DEBUG_HPP
#define DIGGLER_RENDER_GL_DEBUG_HPP

#include "OpenGL.hpp"

namespace diggler {
namespace render {
namespace gl {

class Debug {
public:
  static void APIENTRY glDebugCallback(GLenum source, GLenum type, GLuint id,
    GLenum severity, GLsizei length, const GLchar *message, const void *userParam);
  static void enable();
  static void disable();
};

}
}
}

#endif /* DIGGLER_RENDER_GL_DEBUG_HPP */
