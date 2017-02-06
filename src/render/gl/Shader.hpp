#ifndef DIGGLER_RENDER_GL_SHADER_HPP
#define DIGGLER_RENDER_GL_SHADER_HPP

#include <string>
#include <vector>

#include <epoxy/gl.h>

namespace Diggler {
namespace Render {
namespace gl {

class Shader {
private:
  GLuint id;
  GLint compiled = GL_FALSE;
  const std::vector<std::string> *m_preludeLines;

public:
  enum class Type : GLenum {
    FRAGMENT = GL_FRAGMENT_SHADER,
    VERTEX = GL_VERTEX_SHADER
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
