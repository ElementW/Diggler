#ifndef DIGGLER_RENDER_GL_SHADER_HPP
#define DIGGLER_RENDER_GL_SHADER_HPP

#include <string>
#include <vector>

#include "OpenGL.hpp"

namespace diggler {
namespace render {
namespace gl {

class Shader {
private:
  GLuint id;
  const std::vector<std::string> *m_preludeLines;

public:
  enum class Type : unsigned int {
    FRAGMENT = static_cast<unsigned int>(GL_FRAGMENT_SHADER),
    VERTEX = static_cast<unsigned int>(GL_VERTEX_SHADER)
  } type;
  Shader(Type type);
  Shader(Type type, const std::string& path);
  ~Shader();

  void setPreludeLines(const std::vector<std::string>&);
  bool compileFromFile(const std::string& path);
  bool compileFromString(const std::string& source, const std::string& path = "<source>");
  std::string getError() const;
  GLuint getId() const;
  operator GLuint() const { return getId(); }
  
};

}
}
}

#endif /* DIGGLER_RENDER_GL_SHADER_HPP */
