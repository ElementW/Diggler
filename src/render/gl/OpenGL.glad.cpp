#include "OpenGL.hpp"

#include <stdexcept>

namespace Diggler {
namespace Render {
namespace gl {

void OpenGL::init() {
  if (!gladLoadGL()) {
    throw std::runtime_error("GLAD initialization failed");
  }
  probeExtensions();
}

std::set<OpenGL::Version> OpenGL::loaderSupportedGLVersions() {
  return {};
}

OpenGL::Version OpenGL::version() {
  const int major = GLVersion.major, minor = GLVersion.minor;
  bool isForward = false;
#ifdef GL_CONTEXT_FLAGS
  if (major >= 3) {
    GLuint contextFlags;
    glGetIntegerv(GL_CONTEXT_FLAGS, reinterpret_cast<GLint*>(&contextFlags));
    isForward = bool(contextFlags & GLuint(GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT));
  }
#endif
  bool isCore = false;
#ifdef GL_CONTEXT_PROFILE_MASK
  if (major >= 3 and (major >= 4 or minor >= 2)) {
    GLuint profileFlags = 0;
    glGetIntegerv(GL_CONTEXT_PROFILE_MASK, reinterpret_cast<GLint*>(&profileFlags));
    isCore = bool(profileFlags & GLuint(GL_CONTEXT_CORE_PROFILE_BIT));
  }
#endif
  return { major, minor, false, isCore, isForward };
}

const char* OpenGL::loaderName() {
  return "GLAD";
}

const char* OpenGL::loaderVersion() {
  return "rolling";
}

}
}
}
