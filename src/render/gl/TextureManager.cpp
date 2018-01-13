#include "TextureManager.hpp"

#include "../../util/ColorUtil.hpp"
#include "Texture.hpp"

namespace diggler {
namespace render {
namespace gl {

TextureManager::TextureManager(Game &G) :
  G(G) {
}

TextureManager::~TextureManager() {
}

TextureRef TextureManager::createTexture(uint w, uint h, PixelFormat format,
      byte defaultR, byte defaultG, byte defaultB, byte defaultA) {
  std::unique_ptr<byte[]> data = std::make_unique<byte[]>(w * h * PixelFormatByteSize(format));
  switch (format) {
  case PixelFormat::RGB:
    Util::ColorUtil::fillRGB888(data.get(), defaultR, defaultG, defaultB, w * h);
    break;
  case PixelFormat::RGBA:
    Util::ColorUtil::fillRGBA8888(data.get(), defaultR, defaultG, defaultB, defaultA, w * h);
    break;
  }
  return std::make_shared<Texture>(w, h, format, data.get());
}

TextureRef TextureManager::createTexture(uint w, uint h, PixelFormat format,
      std::unique_ptr<const uint8_t[]> &&data) {
  return std::make_shared<Texture>(w, h, format, data.get());
}

uint64 TextureManager::minUsedVideoMem() const {
  // TODO implement GL TextureManager::minUsedVideoMem
  return 0;
}

}
}
}
