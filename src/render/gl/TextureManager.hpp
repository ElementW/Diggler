#ifndef DIGGLER_RENDER_GL_TEXTURE_MANAGER_HPP
#define DIGGLER_RENDER_GL_TEXTURE_MANAGER_HPP

#include <memory>

#include "../TextureManager.hpp"

namespace Diggler {

class Game;

namespace Render {
namespace gl {

class TextureManager : public Render::TextureManager {
private:
  Game &G;

public:
  TextureManager(Game&);
  ~TextureManager();

  TextureRef createTexture(uint w, uint h, PixelFormat format,
      byte defaultR = 128, byte defaultG = 128, byte defaultB = 128, byte defaultA = 255) override;
  TextureRef createTexture(uint w, uint h, PixelFormat format,
      std::unique_ptr<const uint8_t[]> &&data) override;

  uint64 minUsedVideoMem() const override;
};

}
}
}

#endif /* DIGGLER_RENDER_GL_TEXTURE_MANAGER_HPP */
