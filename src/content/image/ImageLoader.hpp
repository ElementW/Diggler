#ifndef DIGGLER_CONTENT_IMAGE_IMAGE_LOADER_HPP
#define DIGGLER_CONTENT_IMAGE_IMAGE_LOADER_HPP

#include <functional>
#include <memory>

#include "../../io/Stream.hpp"
#include "../../PixelFormat.hpp"
#include "ImageFormat.hpp"

namespace Diggler {
namespace Content {
namespace Image {

class ImageLoader final {
public:
  class Loading;

  struct LoadParams {
    /**
     * How frequently @ref onDecoded will be called when the image is being decoded
     */
    enum class ImageUpdateFrequency {
      NoUpdate, /**< Never call @ref onDecoded. */
      ChunkDecoded, /**< Call @ref onDecoded when a chunk of the image is decoded. */
      AnyPixelDecoded /**< Call @ref onDecoded whenever pixels are decoded, no matter how many. */
    } updateFreq;

    /**
     * @brief Image dimensions retrieved callback.
     *
     * Function to call when the dimensions of the image being decoded are known. Called once.
     *
     * @note May be an empty / default function.
     */
    std::function<void(std::shared_ptr<Loading>)> onDimensionsKnown;

    /**
     * @brief Image part decode callback.
     *
     * Function to call when a part of the image is decoded, depending on the @ref updateFreq
     * setting.
     *
     * Not called when the image decodes fully in one time, or when the image finishes its last
     * decoding batch.
     *
     * @note May be an empty / default function.
     */
    std::function<void(std::shared_ptr<Loading>, uint x, uint y, uint w, uint h)> onDecoded;

    /**
     * @brief Image loaded callback.
     *
     * Function to call when the image is fully loaded.
     */
    std::function<void(std::shared_ptr<Loading>)> onFullyLoaded;

    std::shared_ptr<void> userdata;
  };

  class Loading {
  protected:
    Loading(std::unique_ptr<IO::InStream>&&, const LoadParams&);

  public:
    virtual ~Loading();

    std::unique_ptr<IO::InStream> stream;
    LoadParams loadParams;
    uint w, h;
    byte *pixels;
  };

  static std::shared_ptr<Loading> load(Format, const std::string &path, PixelFormat pixFormat,
      const LoadParams&);

  static std::shared_ptr<Loading> load(Format, std::unique_ptr<IO::InStream> &&stream,
      PixelFormat pixFormat, const LoadParams&);
};

}
}
}

#endif /* DIGGLER_CONTENT_IMAGE_IMAGE_LOADER_HPP */
