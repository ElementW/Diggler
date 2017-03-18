#include "STBImageLoader.hpp"

#include <thread>

#include <stb_image.h>

namespace Diggler {
namespace Content {
namespace Image {
namespace Formats {

static int stream_read(void *user, char *data, int size) {
  IO::InStream &stream = *reinterpret_cast<IO::InStream*>(user);
  stream.readData(data, size);
  return size;
}

static void stream_skip(void *user, int n) {
  IO::InStream &stream = *reinterpret_cast<IO::InStream*>(user);
  stream.skip(n);
}

static int stream_eof(void *user) {
  IO::InStream &stream = *reinterpret_cast<IO::InStream*>(user);
  return stream.eos();
}

class STBILoading final : public STBImageLoader::Loading {
public:
  STBILoading(std::unique_ptr<IO::InStream> &&stream, const ImageLoader::LoadParams &lp) :
    Loading(std::move(stream), lp) {
  }

  ~STBILoading();

  std::thread thread;
};

STBILoading::~STBILoading() {
  stbi_image_free(pixels);
}

std::shared_ptr<STBImageLoader::Loading> STBImageLoader::load(Format format,
    std::unique_ptr<IO::InStream> &&stream, PixelFormat pixFormat, const LoadParams &lp) const {
  auto loading = std::make_shared<STBILoading>(std::move(stream), lp);
  loading->thread = std::thread([pixFormat, loading]() {
    stbi_io_callbacks cbs;
    cbs.read = stream_read;
    cbs.skip = stream_skip;
    cbs.eof = stream_eof;
    int width, height, channels;
    int targetChannels = 0;
    switch (pixFormat) {
    case PixelFormat::RGB:
      targetChannels = 3;
      break;
    case PixelFormat::RGBA:
      targetChannels = 4;
      break;
    }
    loading->pixels = stbi_load_from_callbacks(&cbs, loading->stream.get(), &width, &height,
        &channels, targetChannels);
    loading->w = width;
    loading->h = height;
    if (loading->loadParams.onDimensionsKnown) {
      loading->loadParams.onDimensionsKnown(loading);
    }
    loading->loadParams.onFullyLoaded(loading);

    loading->thread.detach();
  });
  return loading;
}

}
}
}
}
