#include "Texture.hpp"

#include <cerrno>
#include <cstdio>
#include <cstring>

#include <stb_image.h>

#include "DelegateGL.hpp"

#define PushBoundTex() GLint currentBoundTex; glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentBoundTex);
#define PopBoundTex() glBindTexture(GL_TEXTURE_2D, currentBoundTex);

namespace Diggler {
namespace Render {
namespace gl {

static GLenum getGlTexFormat(PixelFormat fmt) {
  switch (fmt) {
  case PixelFormat::RGB:
    return GL_RGB;
    break;
  case PixelFormat::RGBA:
    return GL_RGBA;
    break;
  }
  return GL_INVALID_ENUM;
}

Texture::Texture(uint w, uint h, PixelFormat format, const uint8 *data) :
  Diggler::Texture(w, h, format) {
  PushBoundTex();
  glGenTextures(1, &m_id);
  glBindTexture(GL_TEXTURE_2D, m_id);
  const GLenum glFormat = getGlTexFormat(format);
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  glTexImage2D(GL_TEXTURE_2D, 0, glFormat, w, h, 0, glFormat, GL_UNSIGNED_BYTE, data);
  setFiltering(Filter::Nearest, Filter::Nearest);
  PopBoundTex();
}

Texture::~Texture() {
  GLint currentBoundTex; glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentBoundTex);
  if (static_cast<GLuint>(currentBoundTex) == m_id)
    glBindTexture(GL_TEXTURE_2D, 0);
  glDeleteTextures(1, &m_id);
}

GLenum getFilterGlConstant(Texture::Filter filter) {
  switch (filter) {
  case Texture::Filter::Linear:
    return GL_LINEAR;
  case Texture::Filter::Nearest:
    return GL_NEAREST;
  }
  return GL_INVALID_ENUM;
}

void Texture::setFiltering(Filter min, Filter mag) {
  PushBoundTex();
  bind();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, getFilterGlConstant(min));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, getFilterGlConstant(mag));
  PopBoundTex();
}

static GLenum getWrapGlConstant(Texture::Wrapping wrap) {
  switch (wrap) {
  case Texture::Wrapping::ClampEdge:
    return GL_CLAMP_TO_EDGE;
  case Texture::Wrapping::ClampBorder:
    return GL_CLAMP_TO_BORDER;
  case Texture::Wrapping::Repeat:
    return GL_REPEAT;
  case Texture::Wrapping::MirroredRepeat:
    return GL_MIRRORED_REPEAT;
  }
  return GL_INVALID_ENUM;
}

void Texture::setWrapping(Wrapping s, Wrapping t) {
  PushBoundTex();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, getWrapGlConstant(s));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, getWrapGlConstant(t));
  PopBoundTex();
}

void Texture::setTexture(std::unique_ptr<const uint8_t[]> &&data, PixelFormat format) {
  const GLenum glFormat = getGlTexFormat(format);
  DelegateGL::texSubImage2D(m_id,
    0,  // level, 0 = base, no minimap
    0, 0, m_w, m_h,  // x, y, w, h
    glFormat,  // format
    GL_UNSIGNED_BYTE, // type
    std::move(data));
}

void Texture::setTexture(uint w, uint h, std::unique_ptr<const uint8[]> &&data,
      PixelFormat format) {
  const GLenum glFormat = getGlTexFormat(format);
  m_w = w;
  m_h = h;
  m_format = format;
  DelegateGL::texImage2D(m_id, 0, glFormat, w, h, glFormat, GL_UNSIGNED_BYTE, std::move(data));
}

void Texture::setSubTexture(int x, int y, uint w, uint h, std::unique_ptr<const uint8_t[]> &&data,
    PixelFormat format) {
  const GLenum glFormat = getGlTexFormat(format);
  DelegateGL::texSubImage2D(m_id, 0,
    x, y, w, h,
    glFormat, GL_UNSIGNED_BYTE,
    std::move(data));
}

void Texture::getTexture(uint8_t* data) {
  PushBoundTex();
  bind();
  glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
  PopBoundTex();
}

}
}
}
