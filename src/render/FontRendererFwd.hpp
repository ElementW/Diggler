#ifndef DIGGLER_RENDER_FONT_RENDERER_FWD_HPP
#define DIGGLER_RENDER_FONT_RENDERER_FWD_HPP

#include <memory>

namespace Diggler {
namespace Render {

class FontRendererTextBuffer;

using FontRendererTextBufferRef = std::unique_ptr<FontRendererTextBuffer>;

class FontRenderer;

}
}

#endif /* DIGGLER_RENDER_FONT_RENDERER_FWD_HPP */
