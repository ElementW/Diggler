#include "ImageLoader.hpp"

#include <stdexcept>

#include "formats/flif/FLIFLoader.hpp"
#include "formats/jpeg/JPEGLoader.hpp"
#include "formats/png/PNGLoader.hpp"
#include "formats/ImageLoader.hpp"

namespace Diggler {
namespace Content {
namespace Image {

ImageLoader::Loading::Loading(std::unique_ptr<IO::InStream> &&stream, const LoadParams &lp) :
  stream(std::move(stream)),
  loadParams(lp),
  w(0),
  h(0),
  pixels(nullptr) {
}

ImageLoader::Loading::~Loading() {
}

static std::unique_ptr<Formats::ImageLoader> getFormatLoader(Format format) {
  switch (format) {
  case ImageFormats::FLIF:
    return std::make_unique<Formats::FLIF::FLIFLoader>();
  case ImageFormats::JPEG:
    return std::make_unique<Formats::JPEG::JPEGLoader>();
  case ImageFormats::PNG:
    return std::make_unique<Formats::PNG::PNGLoader>();
  }
  throw std::out_of_range("No appropriate image loader found");
}

std::shared_ptr<ImageLoader::Loading> ImageLoader::load(Format format, const std::string &path,
    PixelFormat pixFormat, const LoadParams &lp) {
  return getFormatLoader(format)->load(format, path, pixFormat, lp);
}

std::shared_ptr<ImageLoader::Loading> ImageLoader::load(Format format,
    std::unique_ptr<IO::InStream> &&stream, PixelFormat pixFormat, const LoadParams &lp) {
  return getFormatLoader(format)->load(format, std::move(stream), pixFormat, lp);
}

}
}
}
