#ifndef DIGGLER_RENDER_GL_FBO_HPP
#define DIGGLER_RENDER_GL_FBO_HPP

#include "../../Platform.hpp"

#include <vector>

#include <epoxy/gl.h>

#include "../../Texture.hpp"

namespace Diggler {
namespace Render {
namespace gl {

class FBO {
private:
  bool m_hasStencil;

public:
  Texture *tex;
  GLuint id, rboId;

  FBO(int w = 640, int h = 480, Texture::PixelFormat format = Texture::PixelFormat::RGB, bool stencil = false);
  ~FBO();

  operator GLuint() const { return id; }

  void resize(int w, int h);

  inline void bind() {
    glBindFramebuffer(GL_FRAMEBUFFER, id);
  }

  inline void unbind() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
  }

  inline bool hasStencil() const {
    return m_hasStencil;
  }
};

}
}
}

#endif /* DIGGLER_RENDER_GL_FBO_HPP */
