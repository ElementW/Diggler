#include "OpenGL.hpp"

#include <glbinding/Binding.h>
#include <glbinding/ContextInfo.h>
#include <glbinding/Meta.h>
#include <glbinding/glbinding-version.h>
#include <glbinding/Version.h>

namespace Diggler {
namespace Render {
namespace gl {

void OpenGL::init() {
  glbinding::Binding::initialize(false);
  probeExtensions();
}

std::set<OpenGL::Version> OpenGL::loaderSupportedGLVersions() {
  std::set<Version> versions;
  for (const auto& version : glbinding::Meta::versions()) {
    versions.emplace(version.majorVersion(), version.minorVersion(), false, false, false);
    if (version.majorVersion() >= 3) {
      versions.emplace(version.majorVersion(), version.minorVersion(), false, false, true);
      versions.emplace(version.majorVersion(), version.minorVersion(), false, true, false);
      versions.emplace(version.majorVersion(), version.minorVersion(), false, true, true);
    }
  }
  return versions;
}

OpenGL::Version OpenGL::version() {
  auto ver = glbinding::ContextInfo::version();
  const int major = ver.majorVersion(), minor = ver.minorVersion();
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
  return "glbinding";
}

const char* OpenGL::loaderVersion() {
  return GLBINDING_VERSION;
}

}
}
}
