#ifndef DIGGLER_RENDER_GL_PROGRAM_HPP
#define DIGGLER_RENDER_GL_PROGRAM_HPP

#include "Shader.hpp"

namespace diggler {
namespace render {
namespace gl {

class Program {
private:
  Shader *vsh, *fsh;
  GLuint id;
  GLboolean linked;
  bool mustDestroy = false;
  std::string fshPath, vshPath;

public:
  /**
   * Creates an OpenGL Program from two existent shaders
   * (shaders dtors won't be called when ~Program is called)
   */
  Program(Shader *vsh, Shader *fsh);

  /**
   * Creates an OpenGL Program shaders read from paths
   * (shaders dtors will be called when ~Program is called)
   */
  Program(const std::string &vshPath, const std::string &fshPath);

  /**
   * Destroys the shader, freeing OpenGL resources and the subsequent Shaders (if needed)
   */
  ~Program();

  /**
   * Set shaders' prelude lines
   * @see Shader::setPreludeLines(const std::vector<std::string>&);
   */
  void setPreludeLines(const std::vector<std::string>&);

  /**
   * Links the shaders together
   * @returns true on success, otherwise false
   * @see getError() returns the error message
   */
  bool link();

  /**
   * @returns The error message generated during link
   * @see link()
   */
  std::string getError() const;

  GLuint getId() const;
  operator GLuint() const { return getId(); }

  /**
   * @param name Name of the attribute
   * @returns OpenGL attribute ID
   */
  GLint att(const std::string &name) const;

  /**
   * @param name Name of the uniform
   * @returns OpenGL uniform ID
   */
  GLint uni(const std::string &name) const;

  /**
   * Makes this Program active
   */
  void bind() const;

  GLuint getFShId() const;
  GLuint getVShId() const;
};

}
}
}

#endif /* DIGGLER_RENDER_GL_PROGRAM_HPP */
