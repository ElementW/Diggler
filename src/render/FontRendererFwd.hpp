#ifndef DIGGLER_RENDER_FONT_RENDERER_FWD_HPP
#define DIGGLER_RENDER_FONT_RENDERER_FWD_HPP

#include <memory>

#include "../platform/Types.hpp"

namespace diggler {
namespace render {

enum class FontRendererTextBufferUsage : uint8 {
  Static,
  Dynamic,
  Stream,

  Default = Static
};

class FontRendererTextBuffer;

using FontRendererTextBufferRef = std::unique_ptr<FontRendererTextBuffer>;

class FontRenderer;

}
}

#endif /* DIGGLER_RENDER_FONT_RENDERER_FWD_HPP */
