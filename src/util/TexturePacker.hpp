#ifndef DIGGLER_UTIL_TEXTURE_PACKER_HPP
#define DIGGLER_UTIL_TEXTURE_PACKER_HPP

#include <memory>
#include <vector>

#include "../platform/Types.hpp"
#include "../Texture.hpp"

namespace diggler {

class Game;

namespace Util {

class TexturePacker {
public:
  struct Coord {
    uint16 x, y, u, v;
  };
  std::vector<Coord> coords;
  int atlasWidth, atlasHeight;

private:
  std::unique_ptr<uint8[]> m_defaultTexture;

  std::unique_ptr<uint8[]> atlasData;

  std::shared_ptr<Texture> atlasTex;
  bool m_freezeTexUpdate;
  void updateTex();

  // No copy
  TexturePacker(const TexturePacker&) = delete;
  TexturePacker& operator=(const TexturePacker&) = delete;

public:
  TexturePacker(Game&, uint w, uint h);
  ~TexturePacker();

  Coord add(const std::string &path);
  Coord add(int width, int height, int channels, const uint8* data);

  void freezeTexUpdate(bool);
  const std::shared_ptr<Texture> getAtlas() {
    return atlasTex;
  }
};

}
}

#endif /* DIGGLER_UTIL_TEXTURE_PACKER */
