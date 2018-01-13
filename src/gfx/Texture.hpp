#ifndef DIGGLER_GFX_TEXTURE_HPP
#define DIGGLER_GFX_TEXTURE_HPP

namespace diggler {
namespace gfx {

class Texture {
public:
  enum class PixelFormat {
    RGB888,
    RGBA8888
  };

  Texture& operator=(const Texture&) = delete;
  Texture& operator=(Texture&&) = delete;

  virtual ~Texture() = 0;
};

namespace cmd {
struct TextureCreate : public ReturnCommand<Texture> {
  unsigned
};
}

}
}

#endif /* DIGGLER_GFX_TEXTURE_HPP */
