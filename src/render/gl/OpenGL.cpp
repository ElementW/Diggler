#include "OpenGL.hpp"

#include <sstream>

namespace Diggler {
namespace Render {
namespace gl {

static std::set<std::string> extensionsSet;

void OpenGL::probeExtensions() {
  if (version() >= 30) {
#ifdef GL_NUM_EXTENSIONS
    GLint numExt;
    glGetIntegerv(GL_NUM_EXTENSIONS, &numExt);
    for (GLint i = 0; i < numExt; ++i) {
      extensionsSet.insert(reinterpret_cast<const char*>(glGetStringi(GL_EXTENSIONS, GLuint(i))));
    }
#endif
  } else {
    std::stringstream ss;
    ss.str(reinterpret_cast<const char*>(glGetString(GL_EXTENSIONS)));
    std::string item;
    while (std::getline(ss, item, ' ')) {
      extensionsSet.insert(item);
    }
  }
}

const std::set<std::string>& OpenGL::extensions() {
  return extensionsSet;
}

bool OpenGL::hasExtension(const std::string &ext) {
  return extensionsSet.find(ext) != extensionsSet.end();
}

}
}
}
