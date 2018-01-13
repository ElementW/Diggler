#ifndef DIGGLER_CONTENT_IMAGE_FORMATS_IMAGE_LOADER_HPP
#define DIGGLER_CONTENT_IMAGE_FORMATS_IMAGE_LOADER_HPP

#include <memory>

#include "../../../io/Stream.hpp"
#include "../../../PixelFormat.hpp"
#include "../ImageLoader.hpp"

namespace diggler {
namespace content {
namespace Image {
namespace Formats {

class ImageLoader {
public:
  using LoadParams = Image::ImageLoader::LoadParams;
  using Loading = Image::ImageLoader::Loading;

  virtual ~ImageLoader();

  virtual std::shared_ptr<Loading> load(Format, const std::string &path, PixelFormat pixFormat,
      const LoadParams&) const = 0;

  virtual std::shared_ptr<Loading> load(Format, std::unique_ptr<io::InStream> &&stream,
      PixelFormat pixFormat, const LoadParams&) const = 0;
};

}
}
}
}

#endif /* DIGGLER_CONTENT_IMAGE_FORMATS_IMAGE_LOADER_HPP */
