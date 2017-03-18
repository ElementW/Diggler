#include "Texture.hpp"

namespace Diggler {

uint Texture::requiredBufferSize() {
  uint texelSize;
  switch (m_format) {
  case PixelFormat::RGB:
    texelSize = 3;
    break;
  case PixelFormat::RGBA:
    texelSize = 4;
    break;
/*  case PixelFormat::Monochrome8:
    texelSize = 1;
    break; */
  }
  return m_w * m_h * texelSize;
}

}
