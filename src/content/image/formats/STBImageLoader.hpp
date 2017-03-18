#ifndef DIGGLER_CONTENT_IMAGE_FORMATS_STB_IMAGE_LOADER_HPP
#define DIGGLER_CONTENT_IMAGE_FORMATS_STB_IMAGE_LOADER_HPP

#include <memory>
#include <string>

#include "ImageLoader.hpp"

#include "../../../io/FileStream.hpp"

namespace Diggler {
namespace Content {
namespace Image {
namespace Formats {

class STBImageLoader final : public ImageLoader {
public:
  std::shared_ptr<Loading> load(Format format, const std::string &path, PixelFormat pixFormat,
      const LoadParams &lp) const override {
    return load(format, std::make_unique<IO::InFileStream>(path), pixFormat, lp);
  }

  std::shared_ptr<Loading> load(Format, std::unique_ptr<IO::InStream> &&stream, PixelFormat format,
      const LoadParams&) const override;
};

}
}
}
}

#endif /* DIGGLER_CONTENT_IMAGE_FORMATS_STB_IMAGE_LOADER_HPP */
