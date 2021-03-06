#ifndef DIGGLER_RENDER_FONT_RENDERER_HPP
#define DIGGLER_RENDER_FONT_RENDERER_HPP

#include "FontRendererFwd.hpp"

#include "RenderParams.hpp"

#include "../platform/PreprocUtils.hpp"
#include "../ui/Font.hpp"

namespace diggler {
namespace render {

class FontRendererTextBuffer {
protected:
  FontRendererTextBuffer() {}

public:
  struct Vertex {
    int16 x, y;
    float tx, ty;
    float r, g, b ,a;
  };

  nocopymove(FontRendererTextBuffer);
  virtual ~FontRendererTextBuffer() = 0;
};

inline FontRendererTextBuffer::~FontRendererTextBuffer() {}

class FontRenderer {
protected:
  inline uintptr_t getRendererData(const ui::Font &f) const {
    return f.rendererData;
  }
  inline void setRendererData(ui::Font &f, uintptr_t data) const {
    f.rendererData = data;
  }

public:
  using TextBuffer = FontRendererTextBuffer;
  using TextBufferRef = FontRendererTextBufferRef;

  FontRenderer() {}
  virtual ~FontRenderer() = 0;

  virtual void registerFont(ui::Font&) = 0;
  virtual void unregisterFont(ui::Font&) = 0;

  virtual TextBufferRef createTextBuffer(
      FontRendererTextBufferUsage = FontRendererTextBufferUsage::Default) = 0;
  virtual void updateTextBuffer(TextBufferRef&, const TextBuffer::Vertex *vertices,
      uint vertexCount) = 0;

  virtual void render(const ui::Font&, const TextBufferRef&, const glm::mat4&) = 0;
};

inline FontRenderer::~FontRenderer() {}

}
}

#endif /* DIGGLER_RENDER_FONT_RENDERER_HPP */
