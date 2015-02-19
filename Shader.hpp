#ifndef SHADER_HPP
#define SHADER_HPP
#include <string>
#include <GL/glew.h>

namespace Diggler {

class Shader {
private:
	GLuint id;
	GLint compiled = GL_FALSE;

public:
	enum class Type : GLenum {
		FRAGMENT = GL_FRAGMENT_SHADER,
		VERTEX = GL_VERTEX_SHADER
	} type;
	Shader(Type type);
	Shader(Type type, const std::string& path);
	bool compileFromFile(const std::string& path);
	bool compileFromString(const std::string& source);
	std::string getError() const;
	GLuint getId() const;
	operator GLuint() const { return getId(); }
	~Shader();
};

}

#endif