#include "Program.hpp"
#include "Platform.hpp"
#include <thread>

namespace Diggler {

Program::Program(Shader* vsh, Shader* fsh) : vsh(vsh), fsh(fsh), id(0) {
	
}

Program::Program(const std::string& vshPath, const std::string& fshPath) : id(0) {
#if DEBUG
	this->fshPath = fshPath;
	this->vshPath = vshPath;
#endif
	vsh = new Shader(Shader::Type::VERTEX, vshPath);
	fsh = new Shader(Shader::Type::FRAGMENT, fshPath);
	mustDestroy = true;
	
	//getDebugStream() << id << ':' << vsh->getError() << fsh->getError()<< std::endl;
}

bool Program::link() {
	id = glCreateProgram();
	glAttachShader(id, vsh->getId());
	glAttachShader(id, fsh->getId());
	glLinkProgram(id);
	glGetProgramiv(id, GL_LINK_STATUS, &linked);
	if (!linked) {
		getErrorStream() << id << ':' << getError() << std::endl;
		return false;
	}
	return true;
}

std::string Program::getError() const {
	GLint log_length = 0;
	glGetProgramiv(id, GL_INFO_LOG_LENGTH, &log_length);
	char* log = (char*)malloc(log_length);
	glGetProgramInfoLog(id, log_length, NULL, log);
	std::string ret(log);
	free(log);
	return ret;
}

GLuint Program::getId() const {
	return id;
}

GLint Program::att(const std::string &name) const {
	if (!linked) {
		getErrorStream() << id << ":Not linked, failed attrib " << name << std::endl;
		return 0;
	}
	GLint loc = glGetAttribLocation(id, name.c_str());
	if (loc == -1) {
#if DEBUG
		getErrorStream() << vshPath << ":Couldn't bind attrib " << name << std::endl;
#else
		getErrorStream() << id << ":Couldn't bind attrib " << name << std::endl;
#endif
		return 0;
	}
	return loc;
}

GLint Program::uni(const std::string &name) const {
	if (!linked) {
		getErrorStream() << id << ":Not linked, failed uniform " << name << std::endl;
		return 0;
	}
	GLint loc = glGetUniformLocation(id, name.c_str());
	if (loc == -1) {
#if DEBUG
		getErrorStream() << vshPath << ":Couldn't bind uniform " << name << std::endl;
#else
		getErrorStream() << id << ":Couldn't bind uniform " << name << std::endl;
#endif
		return 0;
	}
	return loc;
}

void Program::bind() const {
	glUseProgram(id);
}

Program::~Program() {
	if (mustDestroy) {
		delete vsh; delete fsh;
	}
}

}