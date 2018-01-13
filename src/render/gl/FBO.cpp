#include "FBO.hpp"

#include "../../util/Log.hpp"
#include "FeatureSupport.hpp"
#include "Util.hpp"

namespace diggler {
namespace render {
namespace gl {

using Util::Log;
using namespace Util::Logging::LogLevels;

static const char *TAG = "FBO";

FBO::FBO(int w, int h, PixelFormat format, bool stencil) : m_hasStencil(stencil) {
  BoundBufferSave<GL_RENDERBUFFER> saveRbo;
  BoundBufferSave<GL_FRAMEBUFFER> saveFbo;

  glGetError(); // Flush previous errors

  // Set up texture to render color to
  tex = std::make_unique<Texture>(w, h, format);
  glGenFramebuffers(1, &id);
  glBindFramebuffer(GL_FRAMEBUFFER, id);

  // Set up renderbuffer to which depth/stencil will be written to
  glGenRenderbuffers(1, &rboId);
  glBindRenderbuffer(GL_RENDERBUFFER, rboId);
  if (stencil) {
#ifdef GL_DEPTH24_STENCIL8_OES
    const GLenum depth24stencil8 = FeatureSupport::FBO_ARB ? GL_DEPTH24_STENCIL8 :
        GL_DEPTH24_STENCIL8_OES;
#else
    constexpr GLenum depth24stencil8 = GL_DEPTH24_STENCIL8;
#endif
    glRenderbufferStorage(GL_RENDERBUFFER, depth24stencil8, w, h);
  } else {
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, w, h);
  }

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, *tex, 0);
  if (stencil) {
    if (FeatureSupport::FBO_ARB) {
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER,
        rboId);
    } else {
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboId);
      glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rboId);
    }
  } else {
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboId);
  }

  GLenum glErr = glGetError();
  if (glErr != GL_NO_ERROR) {
    Log(Error, TAG) << getErrorString(glErr);
  }
  glErr = glCheckFramebufferStatus(GL_FRAMEBUFFER);
  if (glErr != GL_FRAMEBUFFER_COMPLETE) {
    Log(Error, TAG) << getErrorString(glErr);
  }
}

void FBO::resize(int w, int h) {
  BoundBufferSave<GL_RENDERBUFFER> saveRbo;

  glBindRenderbuffer(GL_RENDERBUFFER, rboId);
  if (m_hasStencil)
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
  else
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, w, h);

  auto format = tex->pixelFormat();
  tex->setTexture(w, h, std::make_unique<uint8[]>(w * h * PixelFormatByteSize(format)), format);
}

FBO::~FBO() {
  glDeleteFramebuffers(1, &id);
  tex.reset();
  glDeleteRenderbuffers(1, &rboId);
}

}
}
}
