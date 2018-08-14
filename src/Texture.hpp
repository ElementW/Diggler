#ifndef DIGGLER_TEXTURE_HPP
#define DIGGLER_TEXTURE_HPP

#include <memory>

#include "PixelFormat.hpp"
#include "platform/Types.hpp"

// TODO: remove me
#include "render/gl/OpenGL.hpp"

namespace diggler {

class Texture {
public:
  enum class Filter {
    Nearest,
    Linear
  };
  enum class Wrapping {
    ClampEdge,
    ClampBorder,
    Repeat,
    MirroredRepeat
  };

protected:
  uint m_w, m_h;
  PixelFormat m_format;

  Texture(uint w, uint h, PixelFormat format) :
    m_w(w),
    m_h(h),
    m_format(format) {
  }

public:
  virtual ~Texture() = 0;

  uint w() const {
    return m_w;
  }

  uint h() const {
    return m_h;
  }

  PixelFormat pixelFormat() const {
    return m_format;
  }

  uint requiredBufferSize();

  virtual void getTexture(uint8 *data) = 0;

  virtual void setTexture(std::unique_ptr<const uint8[]> &&data, PixelFormat format) = 0;

  virtual void setTexture(uint w, uint h, std::unique_ptr<const uint8[]> &&data,
      PixelFormat format) = 0;

  virtual void setSubTexture(int x, int y, uint w, uint h, std::unique_ptr<const uint8[]> &&data,
      PixelFormat format) = 0;

  virtual void setFiltering(Filter min, Filter mag) = 0;

  virtual void setWrapping(Wrapping s, Wrapping t) = 0;
  inline void setWrapping(Wrapping w) {
    setWrapping(w, w);
  }

  // TODO: remove me
  virtual void bind() const = 0;
  // TODO: remove me
  static void unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
  }
};

inline Texture::~Texture() {}

}

#endif /* DIGGLER_TEXTURE_HPP */
