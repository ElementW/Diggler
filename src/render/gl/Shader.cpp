#include "Shader.hpp"

#include <algorithm>
#include <memory>
#include <sstream>

#include "../../Platform.hpp"

namespace Diggler {
namespace Render {
namespace gl {

Shader::Shader(Type type) :
  m_preludeLines(nullptr),
  type(type) {
  id = glCreateShader(GLenum(type));
}

Shader::Shader(Shader::Type type, const std::string &path) :
  m_preludeLines(nullptr) {
  id = glCreateShader(GLenum(type));
  compileFromFile(path);
}

Shader::~Shader() {
  glDeleteShader(id);
}

void Shader::setPreludeLines(const std::vector<std::string> &lines) {
  m_preludeLines = &lines;
}

bool Shader::compileFromFile(const std::string &path) {
  return compileFromString(fs::readFile(path), path);
}

bool Shader::compileFromString(const std::string &source, const std::string &path) {
  if (source.size() == 0)
    return false;
  std::ostringstream oss;
  oss << "#version 100\n";
  if (m_preludeLines) {
    for (const std::string &line : *m_preludeLines) {
      oss << line << "\n";
    }
  }
  oss << "#line 1\n";
  oss << source;
  // Beware of the lifetime, we use c_str() afterwards!
  std::string srcStr = oss.str();
  const char *src = srcStr.c_str();
  glShaderSource(id, 1, &src, nullptr);
  glCompileShader(id);
  GLint compiled;
  glGetShaderiv(id, GL_COMPILE_STATUS, &compiled);
  if (!GLboolean(compiled)) {
    getErrorStream() << "Compile error in " << path << "\n" << getError() << std::endl;
    return false;
  }
  return true;
}

std::string Shader::getError() const {
  GLint logLength = 0;
  glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLength);
  if (logLength < 1)
    return "[empty error string]";
  std::unique_ptr<char[]> log = std::make_unique<char[]>(logLength);
  glGetShaderInfoLog(id, logLength, NULL, log.get());
  std::string ret(log.get());
  return ret;
}

GLuint Shader::getId() const {
  return id;
}

}
}
}
