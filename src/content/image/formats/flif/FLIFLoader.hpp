#ifndef DIGGLER_CONTENT_IMAGE_FORMATS_FLIF_LOADER_HPP
#define DIGGLER_CONTENT_IMAGE_FORMATS_FLIF_LOADER_HPP

#include "../ImageLoader.hpp"

#include "../../../../io/FileStream.hpp"

namespace Diggler {
namespace Content {
namespace Image {
namespace Formats {
namespace FLIF {

class FLIFLoader final : public ImageLoader {
public:
  std::shared_ptr<Loading> load(Format format, const std::string &path, PixelFormat pixFormat,
      const LoadParams &lp) const override {
    return load(format, std::make_unique<IO::InFileStream>(path), pixFormat, lp);
  }

  std::shared_ptr<Loading> load(Format, std::unique_ptr<IO::InStream> &&stream,
      PixelFormat pixFormat, const LoadParams&) const override;
};

}
}
}
}
}

#endif /* DIGGLER_CONTENT_IMAGE_FORMATS_FLIF_LOADER_HPP */
