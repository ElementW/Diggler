#ifndef FONT_HPP
#define FONT_HPP

#include <memory>

#include "../platform/types/mat4.hpp"
#include "../platform/Types.hpp"
#include "../render/FontRendererFwd.hpp"

namespace diggler {

class Texture;
class Program;
class Game;

namespace ui {

class FontManager;

class Font {
private:
  friend class render::FontRenderer;
  uintptr_t rendererData;

  std::shared_ptr<Texture> m_texture;
  struct CoordInfo {
    float x1, y1, x2, y2;
  } *m_coords;
  struct CharTexPos {
    int width;
    float left, right;
  } *texPos;
  uint8 height;
  Game *G;

  Font(const Font &other) = delete;

public:
  struct Size { int x, y; };

  Font(Game *G, const std::string &path);
  ~Font();

  render::FontRendererTextBufferRef createTextBuffer() const;
  render::FontRendererTextBufferRef createTextBuffer(render::FontRendererTextBufferUsage) const;
  void updateTextBuffer(render::FontRendererTextBufferRef&, const std::string &text) const;

  void draw(const render::FontRendererTextBufferRef&, const mat4 &matrix) const;
  Size getSize(const std::string &text) const;
  int getHeight() const;
  const std::shared_ptr<Texture> texture() const {
    return m_texture;
  }
};

}
}

#endif
