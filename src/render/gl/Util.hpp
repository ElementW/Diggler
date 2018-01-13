#ifndef DIGGLER_RENDER_GL_UTIL_HPP
#define DIGGLER_RENDER_GL_UTIL_HPP

#include "../../Platform.hpp"

#include "OpenGL.hpp"

namespace diggler {
namespace render {
namespace gl {

const char* getErrorString(GLenum code);

inline void getInteger(GLenum src, GLint &dst) {
  glGetIntegerv(src, &dst);
}

inline GLint getInteger(GLenum src) {
  GLint dst;
  glGetIntegerv(src, &dst);
  return dst;
}

inline void getUInteger(GLenum src, GLuint &dst) {
  glGetIntegerv(src, reinterpret_cast<GLint*>(&dst));
}

inline GLuint getUInteger(GLenum src) {
  GLuint dst;
  glGetIntegerv(src, reinterpret_cast<GLint*>(&dst));
  return dst;
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
    case GL_VERTEX_ARRAY:
      return GL_VERTEX_ARRAY_BINDING;
    default:
      break;
  }
  return GL_INVALID_ENUM;
}

struct Bound2DTextureSave {
  GLuint texture;

  Bound2DTextureSave() {
    getUInteger(GL_TEXTURE_BINDING_2D, texture);
  }

  inline ~Bound2DTextureSave() {
    glBindTexture(GL_TEXTURE_2D, texture);
  }
};

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
template<> inline BoundBufferSave<GL_VERTEX_ARRAY>::~BoundBufferSave() {
  glBindVertexArray(buffer);
}

}
}
}

#endif /* DIGGLER_RENDER_GL_UTIL_HPP */
