#include "Shader.hpp"
#include "Platform.hpp"
#include <sstream>
#include <algorithm>

namespace Diggler {

Shader::Shader(Type type) : srcDefines(nullptr), type(type) {
	id = glCreateShader((GLenum)type);
}

Shader::Shader(Shader::Type type, const std::string &path) : srcDefines(nullptr) {
	id = glCreateShader((GLenum)type);
	compileFromFile(path);
}

Shader::~Shader() {
	delete[] srcDefines;
	glDeleteShader(id);
}

void Shader::setDefines(const std::vector<std::string> &defs) {
	delete[] srcDefines;
	if (defs.size() == 0)
		return;
	std::ostringstream oss;
	for (const std::string &s : defs) {
		oss << "#define " << s << "\n";
	}
	char *strDefines = new char[(uint)oss.tellp()+1];
	std::copy_n(oss.str().c_str(), (uint)oss.tellp()+1, strDefines);
	srcDefines = strDefines;
}

bool Shader::compileFromFile(const std::string &path) {
	return compileFromString(fs::readFile(path), path);
}

bool Shader::compileFromString(const std::string &source, const std::string &path) {
	if (source.size() == 0)
		return false;
	std::ostringstream oss;
	oss << "#version 100\n";
	if (srcDefines)
		oss << srcDefines;
	oss << "#line 1\n";
	oss << source;
	// Beware of the lifetime, we use c_str() afterwards!
	std::string srcStr = oss.str();
	const char *src = srcStr.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);
	glGetShaderiv(id, GL_COMPILE_STATUS, &compiled);
	if (!compiled) {
		getErrorStream() << "Compile error in " << path << "\n" << getError() << std::endl;
		return false;
	}
	return true;
}

std::string Shader::getError() const {
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

GLuint Shader::getId() const {
	return id;
}

}