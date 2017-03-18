#ifndef DIGGLER_CONTENT_TEXTURE_TEXTURE_LOADER_HPP
#define DIGGLER_CONTENT_TEXTURE_TEXTURE_LOADER_HPP

#include <memory>

#include "../../Texture.hpp"
#include "../image/ImageLoader.hpp"

namespace Diggler {

class Game;

namespace Content {
namespace Texture {

class TextureLoader {
public:
  class Loading {
  public:
    using LoadParams = Image::ImageLoader::LoadParams;

    std::shared_ptr<Image::ImageLoader::Loading> imageLoading;
    std::shared_ptr<Diggler::Texture> texture;
  };

  static std::shared_ptr<Loading> load(Game&, Image::Format, const std::string &path,
      PixelFormat pixFormat);

  static std::shared_ptr<Loading> load(Game&, Image::Format, std::unique_ptr<IO::InStream> &&stream,
      PixelFormat pixFormat);
};

}
}
}

#endif /* DIGGLER_CONTENT_TEXTURE_TEXTURE_LOADER_HPP */
