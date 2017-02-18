#ifndef FONT_HPP
#define FONT_HPP

#include "../Platform.hpp"

#include <epoxy/gl.h>
#include <glm/glm.hpp>

#include "../render/FontRendererFwd.hpp"

namespace Diggler {

class Texture;
class Program;
class Game;

namespace UI {

class FontManager;

class Font {
private:
  friend class Render::FontRenderer;
  uintptr_t rendererData;

  Texture *m_texture;
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

  Render::FontRendererTextBufferRef createTextBuffer() const;
  Render::FontRendererTextBufferRef createTextBuffer(Render::FontRendererTextBufferUsage) const;
  void updateTextBuffer(Render::FontRendererTextBufferRef&, const std::string &text) const;

  void draw(const Render::FontRendererTextBufferRef&, const glm::mat4 &matrix) const;
  Size getSize(const std::string &text) const;
  int getHeight() const;
  const Texture* texture() const {
    return m_texture;
  }
};

}
}

#endif
