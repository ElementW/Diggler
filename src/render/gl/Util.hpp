#ifndef DIGGLER_RENDER_GL_UTIL_HPP
#define DIGGLER_RENDER_GL_UTIL_HPP

#include "../../Platform.hpp"

#include <epoxy/gl.h>

namespace Diggler {
namespace Render {
namespace gl {

const char* getErrorString(GLenum code);

inline void getInteger(GLenum src, GLint &dst) {
  glGetIntegerv(src, &dst);
}

inline void getUInteger(GLenum src, GLuint &dst) {
  glGetIntegerv(src, reinterpret_cast<GLint*>(&dst));
}

constexpr GLenum getBinding(GLenum bufferT) {
  switch (bufferT) {
    case GL_ARRAY_BUFFER:
      return GL_ARRAY_BUFFER_BINDING;
    case GL_ELEMENT_ARRAY_BUFFER:
      return GL_ELEMENT_ARRAY_BUFFER_BINDING;
    case GL_FRAMEBUFFER:
      return GL_FRAMEBUFFER_BINDING;
    case GL_RENDERBUFFER:
      return GL_RENDERBUFFER_BINDING;
    default:
      return 0;
  }
}

template<GLenum BufferT, GLenum BoundBufferT = getBinding(BufferT)>
struct BoundBufferSave {
  GLuint buffer;

  BoundBufferSave() {
    getUInteger(BoundBufferT, buffer);
  }
  inline ~BoundBufferSave();
};
template<> inline BoundBufferSave<GL_ARRAY_BUFFER>::~BoundBufferSave() {
  glBindBuffer(GL_ARRAY_BUFFER, buffer);
}
template<> inline BoundBufferSave<GL_ELEMENT_ARRAY_BUFFER>::~BoundBufferSave() {
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
}
template<> inline BoundBufferSave<GL_FRAMEBUFFER>::~BoundBufferSave() {
  glBindFramebuffer(GL_FRAMEBUFFER, buffer);
}
template<> inline BoundBufferSave<GL_RENDERBUFFER>::~BoundBufferSave() {
  glBindRenderbuffer(GL_RENDERBUFFER, buffer);
}

}
}
}

#endif /* DIGGLER_RENDER_GL_UTIL_HPP */
