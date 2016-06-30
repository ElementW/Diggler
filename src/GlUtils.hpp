#ifndef GL_UTILS_HPP
#define GL_UTILS_HPP
#include <string>
#include <epoxy/gl.h>

namespace Diggler {

class GlUtils {
private:
  GlUtils();
public:
  static std::string getErrorString(GLenum code);
};

}

#endif