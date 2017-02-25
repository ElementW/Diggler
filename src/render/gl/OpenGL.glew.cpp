#include "OpenGL.hpp"

#include <stdexcept>

namespace Diggler {
namespace Render {
namespace gl {

void OpenGL::init() {
  GLenum err = glewInit();
  if (err != GLEW_OK) {
    throw std::runtime_error(std::string("GLEW initialization failed") +
                             reinterpret_cast<const char*>(glewGetErrorString(err)));
  }
  probeExtensions();
}

std::set<OpenGL::Version> OpenGL::loaderSupportedGLVersions() {
  std::set<Version> versions;
  for (int ver : { 20, 21, 30, 31, 32, 33, 40, 41, 42, 43, 44, 45 }) {
    versions.emplace(ver / 10, ver % 10, false, false, false);
    if (ver >= 30) {
      versions.emplace(ver / 10, ver % 10, false, false, true);
      versions.emplace(ver / 10, ver % 10, false, true, false);
      versions.emplace(ver / 10, ver % 10, false, true, true);
    }
  }
  return versions;
}

OpenGL::Version OpenGL::version() {
  auto ver = glGetString(GL_VERSION);
  const int major = ver[0] - '0', minor = ver[2] - '0';
  GLuint contextFlags = 0;
  if (major >= 3) {
    glGetIntegerv(GL_CONTEXT_FLAGS, reinterpret_cast<GLint*>(&contextFlags));
  }
  GLuint profileFlags = 0;
  if (major >= 3 and (major >= 4 or minor >= 2)) {
    glGetIntegerv(GL_CONTEXT_PROFILE_MASK, reinterpret_cast<GLint*>(&profileFlags));
  }
  return { major, minor, false,
      bool(profileFlags & GLuint(GL_CONTEXT_CORE_PROFILE_BIT)),
      bool(contextFlags & GLuint(GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT))
  };
}

const char* OpenGL::loaderName() {
  return "GLEW";
}

const char* OpenGL::loaderVersion() {
  return reinterpret_cast<const char*>(glewGetString(GLEW_VERSION));
}

}
}
}
