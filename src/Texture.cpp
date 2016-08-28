#include "Texture.hpp"
#include "Platform.hpp"
#include <stb_image.h>
#include <cstdio>
#include <cerrno>
#include <cstring>
#define PushBoundTex() GLint currentBoundTex; glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentBoundTex);
#define PopBoundTex() glBindTexture(GL_TEXTURE_2D, currentBoundTex);

#define TEXTURE_LOAD_DEBUG 0

namespace Diggler {

Texture::Texture(uint w, uint h, Texture::PixelFormat format) :
  m_w(w),
  m_h(h),
  m_format(format) {
  PushBoundTex();
  create();
  setPlaceholder();
  PopBoundTex();
}

Texture::Texture(uint w, uint h, const uint8_t* data, Texture::PixelFormat format) {
  PushBoundTex();
  create();
  setTexture(w, h, data, format);
  PopBoundTex();
}

static GLenum getGlTexFormat(Texture::PixelFormat fmt);
Texture::Texture(const std::string &path, PixelFormat format) {
  PushBoundTex();
  create();
  int stbiFormat;
  switch (format) {
  case PixelFormat::RGB:
    stbiFormat = STBI_rgb;
    break;
  case PixelFormat::RGBA:
    stbiFormat = STBI_rgb_alpha;
    break;
  case PixelFormat::Monochrome8:
    stbiFormat = STBI_grey;
    break;
  }
  int width, height, channels;
  unsigned char *ptr = nullptr;
  FILE *fp = fopen(path.c_str(), "rb");
  if (fp != nullptr) {
    ptr = stbi_load_from_file(fp, &width, &height, &channels, stbiFormat);
    m_w = width; m_h = height;
  }
  if (ptr && width && height) {
    setTexture(m_w, m_h, ptr, format);
    stbi_image_free(ptr);
#if TEXTURE_LOAD_DEBUG
    getDebugStream() << "Loaded image \"" << path << "\" w=" << w << " h=" << h <<
      " c=" << channels << " pf=" << (int)format << " glPf=" << (int)getGlTexFormat(format) << std::endl;
#endif
  } else {
    m_w = 64; m_h = 64;
    if (fp == nullptr)
      getErrorStream() << "Failed to open \"" << path << "\" : " << std::strerror(errno) << std::endl;
    else
      getErrorStream() << "Failed to load image \"" << path << "\" : " << stbi_failure_reason() << std::endl;
    setPlaceholder();
  }
  PopBoundTex();
}

Texture::~Texture() {
  GLint currentBoundTex; glGetIntegerv(GL_TEXTURE_BINDING_2D, &currentBoundTex);
  if (static_cast<GLuint>(currentBoundTex) == m_id)
    glBindTexture(GL_TEXTURE_2D, 0);
  glDeleteTextures(1, &m_id);
}

void Texture::create() {
  glGenTextures(1, &m_id);
  glBindTexture(GL_TEXTURE_2D, m_id);
  setFiltering(Filter::Nearest, Filter::Nearest);
}

GLenum getFilterGlConstant(Texture::Filter filter) {
  switch (filter) {
  case Texture::Filter::Linear:
    return GL_LINEAR;
  case Texture::Filter::Nearest:
    return GL_NEAREST;
  }
  return 0;
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
  return 0;
}

void Texture::setWrapping(Wrapping s, Wrapping t) {
  PushBoundTex();
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, getWrapGlConstant(s));
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, getWrapGlConstant(t));
  PopBoundTex();
}

void Texture::setPlaceholder() {
  size_t pxLength;
  switch (m_format) {
  case PixelFormat::RGB:
    pxLength = 3;
    break;
  case PixelFormat::RGBA:
    pxLength = 4;
    break;
  case PixelFormat::Monochrome8:
    pxLength = 1;
    break;
  }
  unsigned char *white = new unsigned char[m_w * m_h * pxLength];
  memset(white, 255, m_w * m_h * pxLength);
  setTexture(m_w, m_h, white, m_format);
  delete[] white;
}

static GLenum getGlTexFormat(Texture::PixelFormat fmt) {
  switch (fmt) {
  case Texture::PixelFormat::RGB:
    return GL_RGB;
    break;
  case Texture::PixelFormat::RGBA:
    return  GL_RGBA;
    break;
  case Texture::PixelFormat::Monochrome8:
    return GL_R8;
    break;
  }
  return 0;
}

void Texture::setTexture(uint w, uint h, const uint8_t *data, PixelFormat format) {
  PushBoundTex();
  bind();
  m_w = w; m_h = h;
  this->m_format = format;
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // This. GL = state machine
  GLenum glFormat = getGlTexFormat(format);
  glTexImage2D(GL_TEXTURE_2D, // target
      0,  // level, 0 = base, no minimap
      glFormat, // internalformat
      w,  // width
      h,  // height
      0,  // border
      glFormat,  // format
      GL_UNSIGNED_BYTE, // type
      data);
  PopBoundTex();
}

void Texture::setTexture(const uint8_t *data, PixelFormat format) {
  return setSubTexture(0, 0, m_w, m_h, data, format);
}

void Texture::setSubTexture(int x, int y, uint w, uint h, const uint8_t *data, PixelFormat format) {
  PushBoundTex();
  bind();
  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
  GLenum glFormat = getGlTexFormat(format);
  glTexSubImage2D(GL_TEXTURE_2D, 0,
    x, y, w, h,
    glFormat, GL_UNSIGNED_BYTE,
    data);
  PopBoundTex();
}

void Texture::resize(uint w, uint h) {
  if (m_w == w && m_h == h)
    return;
  PushBoundTex();
  bind();
  GLenum glFormat = getGlTexFormat(m_format);
  glTexImage2D(GL_TEXTURE_2D, // target
    0,  // level, 0 = base, no minimap,
    glFormat, // internalformat
    w,  // width
    h,  // height
    0,  // border
    glFormat,  // format
    GL_UNSIGNED_BYTE, // type
    nullptr);
  m_w = w;
  m_h = h;
  PopBoundTex();
}

Texture::PixelFormat Texture::pixelFormat() {
  return m_format;
}

uint Texture::requiredBufferSize() {
  uint texelSize;
  switch (m_format) {
  case PixelFormat::RGB:
    texelSize = 3;
    break;
  case PixelFormat::RGBA:
    texelSize = 4;
    break;
  case PixelFormat::Monochrome8:
    texelSize = 1;
    break;
  }
  return m_w * m_h * texelSize;
}

void Texture::getTexture(uint8_t* data) {
  PushBoundTex();
  bind();
  glGetTexImage(GL_TEXTURE_2D, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
  PopBoundTex();
}

void Texture::bind(uint number) const {
  glActiveTexture(GL_TEXTURE0+number);
  glBindTexture(GL_TEXTURE_2D, m_id);
  glActiveTexture(GL_TEXTURE0);
}

}
