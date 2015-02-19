#include "Shader.hpp"
#include "Platform.hpp"

Diggler::Shader::Shader(Type type) : type(type) {
	id = glCreateShader((GLenum)type);
}

Diggler::Shader::Shader(Diggler::Shader::Type type, const std::string& path) {
	id = glCreateShader((GLenum)type);
	compileFromFile(path);
}

bool Diggler::Shader::compileFromFile(const std::string& path) {
	return compileFromString(fs::readFile(path));
}

bool Diggler::Shader::compileFromString(const std::string& source) {
	if (source.size() == 0)
		return false;
	const char *src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);
	glGetShaderiv(id, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		getErrorStream() << getError() << std::endl;
		return false;
	}
	return true;
}

std::string Diggler::Shader::getError() const {
	GLint log_length = 0;
	glGetShaderiv(id, GL_INFO_LOG_LENGTH, &log_length);
	if (log_length < 1)
		return "[empty error string]";
	char* log = (char*)malloc(log_length);
	glGetShaderInfoLog(id, log_length, NULL, log);
	std::string ret(log);
	free(log);
	return ret;
}

GLuint Diggler::Shader::getId() const {
	return id;
}

Diggler::Shader::~Shader() {
	glDeleteShader(id);
}
