#ifndef DIGGLER_RENDER_GL_OPENGL_HPP
#define DIGGLER_RENDER_GL_OPENGL_HPP

#include "../../platform/Fixes.hpp"

#include <set>
#include <string>

#if defined(DIGGLER_GL_USE_LIBEPOXY)
  #include <epoxy/gl.h>
#elif defined(DIGGLER_GL_USE_GLAD)
  #include <glad/glad.h>
#elif defined(DIGGLER_GL_USE_GLBINDING)
  #include <glbinding/gl/gl.h>
  using namespace gl;
#elif defined(DIGGLER_GL_USE_GLEW)
  #define GLEW_NO_GLU
  #include <GL/glew.h>
#else
  #error "No OpenGL loader selected"
#endif

namespace diggler {
namespace render {
namespace gl {

class OpenGL {
private:
  static void probeExtensions();

public:
  struct Version {
    int major, minor;
    bool isGLES, isCore, isForward;

    Version(int major, int minor, bool isGLES, bool isCore, bool isForward) :
      major(major),
      minor(minor),
      isGLES(isGLES),
      isCore(isCore),
      isForward(isForward) {}

    operator int() const { return major * 10 + minor; }
    bool operator<(const Version &v) const {
      return major < v.major && minor < v.minor &&
          (isGLES ^ v.isGLES) && (isCore ^ v.isCore) && (isForward ^ v.isForward);
    }
  };

  /**
   * @return The OpenGL Loader name
   */
  static const char* loaderName();

  /**
   * @return The OpenGL Loader version
   */
  static const char* loaderVersion();

  /**
   * Get the list of OpenGL versions whose initialization is supported by the Loader.
   * An empty set means supported versions are unknown.
   * @note Presence of a given version in this liss only shows Loader support, not driver support
   * @return The OpenGL versions the Loader supports
   */
  static std::set<Version> loaderSupportedGLVersions();


  /**
   * Initializes OpenGL entrypoints
   */
  static void init();

  /**
   * @return The currently loaded OpenGL version
   */
  static Version version();

  /**
   * @return The currently available OpenGL extensions
   */
  static const std::set<std::string>& extensions();

  /**
   * Checks for availability of an OpenGL extension
   * @return `true` if the extension is available, `false` otherwise
   */
  static bool hasExtension(const std::string&);
};

}
}
}

#endif /* DIGGLER_RENDER_GL_OPENGL_HPP */
