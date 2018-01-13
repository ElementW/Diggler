#include "OpenGL.hpp"

#include <array>

namespace diggler {
namespace render {
namespace gl {

void OpenGL::init() {
  // libepoxy initializes lazily
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
  for (int ver : { 20, 30, 31, 32 }) {
    versions.emplace(ver / 10, ver % 10, true, false, false);
  }
  return versions;
}

OpenGL::Version OpenGL::version() {
  auto ver = epoxy_gl_version();
  const int major = ver / 10, minor = ver % 10;
  GLuint contextFlags = 0;
  if (major >= 3) {
    glGetIntegerv(GL_CONTEXT_FLAGS, reinterpret_cast<GLint*>(&contextFlags));
  }
  GLuint profileFlags = 0;
  if (major >= 3 and (major >= 4 or minor >= 2)) {
    glGetIntegerv(GL_CONTEXT_PROFILE_MASK, reinterpret_cast<GLint*>(&profileFlags));
  }
  return { major, minor, !epoxy_is_desktop_gl(),
      bool(profileFlags & GL_CONTEXT_CORE_PROFILE_BIT),
      bool(contextFlags & GL_CONTEXT_FLAG_FORWARD_COMPATIBLE_BIT)
  };
}

const char* OpenGL::loaderName() {
  return "libepoxy";
}

const char* OpenGL::loaderVersion() {
  return "rolling";
}

}
}
}
