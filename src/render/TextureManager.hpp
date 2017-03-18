#ifndef DIGGLER_RENDER_TEXTURE_MANAGER_HPP
#define DIGGLER_RENDER_TEXTURE_MANAGER_HPP

#include <memory>

#include "../platform/Types.hpp"
#include "../Texture.hpp"

namespace Diggler {
namespace Render {

using TextureRef = std::shared_ptr<Texture>;
using TextureWeakref = std::weak_ptr<Texture>;

class TextureManager {
public:
  virtual ~TextureManager() = 0;

  virtual TextureRef createTexture(uint w, uint h, PixelFormat format,
      byte defaultR = 128, byte defaultG = 128, byte defaultB = 128, byte defaultA = 255) = 0;
  virtual TextureRef createTexture(uint w, uint h, PixelFormat format,
      std::unique_ptr<const uint8_t[]> &&data) = 0;

  virtual uint64 minUsedVideoMem() const = 0;
};

inline TextureManager::~TextureManager() {}

}
}

#endif /* DIGGLER_RENDER_TEXTURE_MANAGER_HPP */
