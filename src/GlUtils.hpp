#ifndef GL_UTILS_HPP
#define GL_UTILS_HPP
#include <GL/glew.h>
#include <string>

namespace Diggler {

class GlUtils {
private:
	GlUtils();
public:
	static std::string getErrorString(GLenum code);
};

}

#endif