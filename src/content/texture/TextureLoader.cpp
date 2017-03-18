#include "TextureLoader.hpp"

#include "../../Game.hpp"
#include "../../render/Renderer.hpp"
#include "../../util/ColorUtil.hpp"

namespace Diggler {
namespace Content {
namespace Texture {

using Loading = TextureLoader::Loading;
using ILoading = Image::ImageLoader::Loading;
using LoadParams = Image::ImageLoader::LoadParams;

static LoadParams getLoadParams(PixelFormat pixFormat) {
  LoadParams lp;
  lp.updateFreq = LoadParams::ImageUpdateFrequency::ChunkDecoded;
  lp.onDimensionsKnown = [pixFormat](std::shared_ptr<Image::ImageLoader::Loading> il) {
    std::shared_ptr<Loading> l = std::static_pointer_cast<Loading>(il->loadParams.userdata);
    auto data = std::make_unique<byte[]>(il->w * il->h * PixelFormatByteSize(pixFormat));
    switch (pixFormat) {
    case PixelFormat::RGB:
      Util::ColorUtil::fillRGB888(data.get(), 127, 0, 0, il->w * il->h);
      break;
    case PixelFormat::RGBA:
      Util::ColorUtil::fillRGBA8888(data.get(), 127, 0, 0, 255, il->w * il->h);
      break;
    }
    l->texture->setTexture(il->w, il->h, std::move(data), pixFormat);
  };
  lp.onDecoded = [pixFormat](std::shared_ptr<Image::ImageLoader::Loading> l, uint x, uint y, uint w,
      uint h) {
    (void) l; (void) x; (void) y; (void) w; (void) h;
    // TODO onDecoded
  };
  lp.onFullyLoaded = [pixFormat](std::shared_ptr<Image::ImageLoader::Loading> il) {
    std::shared_ptr<Loading> l = std::static_pointer_cast<Loading>(il->loadParams.userdata);
    const uint bytes = il->w * il->h * PixelFormatByteSize(pixFormat);
    auto data = std::make_unique<byte[]>(bytes);
    std::memcpy(data.get(), il->pixels, bytes);
    l->texture->setTexture(std::move(data), pixFormat);
    // Let the ImageLoader::Loading die if it isn't referenced,
    // which in turn will let this TextureLoader::Loading die.
    l->imageLoading.reset();
  };
  return lp;
}

std::shared_ptr<Loading> TextureLoader::load(Game &G, Image::Format format, const std::string &path,
    PixelFormat pixFormat) {
  LoadParams lp = getLoadParams(pixFormat);
  std::shared_ptr<Loading> l = std::make_shared<Loading>();
  lp.userdata = l;
  l->texture = G.R->textureManager->createTexture(1, 1, pixFormat);
  l->imageLoading = Image::ImageLoader::load(format, path, pixFormat, lp);
  return l;
}

std::shared_ptr<Loading> TextureLoader::load(Game &G, Image::Format format,
    std::unique_ptr<IO::InStream> &&stream, PixelFormat pixFormat) {
  LoadParams lp = getLoadParams(pixFormat);
  std::shared_ptr<Loading> l = std::make_shared<Loading>();
  lp.userdata = l;
  l->texture = G.R->textureManager->createTexture(1, 1, pixFormat);
  l->imageLoading = Image::ImageLoader::load(format, std::move(stream), pixFormat, lp);
  return l;
}

}
}
}
