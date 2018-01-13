#ifndef DIGGLER_CONTENT_TEXTURE_TEXTURE_LOADER_HPP
#define DIGGLER_CONTENT_TEXTURE_TEXTURE_LOADER_HPP

#include <memory>

#include "../../Texture.hpp"
#include "../image/ImageLoader.hpp"

namespace diggler {

class Game;

namespace content {
namespace Texture {

class TextureLoader {
public:
  class Loading {
  public:
    using LoadParams = Image::ImageLoader::LoadParams;

    std::shared_ptr<Image::ImageLoader::Loading> imageLoading;
    std::shared_ptr<diggler::Texture> texture;
  };

  static std::shared_ptr<Loading> load(Game&, Image::Format, const std::string &path,
      PixelFormat pixFormat);

  static std::shared_ptr<Loading> load(Game&, Image::Format, std::unique_ptr<io::InStream> &&stream,
      PixelFormat pixFormat);
};

}
}
}

#endif /* DIGGLER_CONTENT_TEXTURE_TEXTURE_LOADER_HPP */
