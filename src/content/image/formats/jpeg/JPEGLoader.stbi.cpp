#include "JPEGLoader.hpp"

#include "../STBImageLoader.hpp"

namespace Diggler {
namespace Content {
namespace Image {
namespace Formats {
namespace JPEG {

std::shared_ptr<JPEGLoader::Loading> JPEGLoader::load(Format format,
    std::unique_ptr<IO::InStream> &&stream, PixelFormat pixFormat, const LoadParams &lp) const {
  return STBImageLoader().load(format, std::move(stream), pixFormat, lp);
}

}
}
}
}
}
