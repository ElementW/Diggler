#include "FBO.hpp"

#include "Util.hpp"

#ifdef IN_IDE_PARSER
  void glCheck();
#else
  #define glCheck() { GLenum glErr = glGetError(); \
    if (glErr) { \
      getDebugStream() << getErrorString(glErr) << std::endl; \
    } glErr = glCheckFramebufferStatus(GL_FRAMEBUFFER); if (glErr != GL_FRAMEBUFFER_COMPLETE) { \
      getDebugStream() << getErrorString(glErr) << std::endl; \
    } \
  }
#endif

namespace Diggler {
namespace Render {
namespace gl {

FBO::FBO(int w, int h, Texture::PixelFormat format, bool stencil) : m_hasStencil(stencil) {
  BoundBufferSave<GL_RENDERBUFFER> saveRbo;
  BoundBufferSave<GL_FRAMEBUFFER> saveFbo;

  glGetError(); // Flush previous errors

  // Set up texture to render color to
  tex = new Texture(w, h, format);
  glGenFramebuffers(1, &id);
  glBindFramebuffer(GL_FRAMEBUFFER, id);

  // Set up renderbuffer to which depth/stencil will be written to
  glGenRenderbuffers(1, &rboId);
  glBindRenderbuffer(GL_RENDERBUFFER, rboId);
  if (stencil)
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
  else
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, w, h);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *tex, 0);
  glFramebufferRenderbuffer(GL_FRAMEBUFFER, stencil ? GL_DEPTH_STENCIL_ATTACHMENT : GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboId);

  glCheck();
}

void FBO::resize(int w, int h) {
  BoundBufferSave<GL_RENDERBUFFER> saveRbo;

  glBindRenderbuffer(GL_RENDERBUFFER, rboId);
  if (m_hasStencil)
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
  else
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, w, h);

  tex->resize(w, h);
}

FBO::~FBO() {
  glDeleteFramebuffers(1, &id);
  delete tex;
  glDeleteRenderbuffers(1, &rboId);
}

}
}
}
