#include "Font.hpp"

#include <fstream>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../content/texture/TextureLoader.hpp"
#include "../Texture.hpp"
#include "../render/Renderer.hpp"
#include "../Game.hpp"

namespace diggler {
namespace ui {

static const struct { float r, g, b; } ColorTable[16] = {
  {1.0f, 1.0f, 1.0f},
  {0.66f, 0.66f, 0.66f},
  {0.33f, 0.33f, 0.33f},
  {0.0f, 0.0f, 0.0f},
  {1.0f, 0.0f, 0.0f},
  {1.0f, 0.5f, 0.0f},
  {1.0f, 1.0f, 0.0f},
  {0.5f, 1.0f, 0.0f},
  {0.0f, 1.0f, 0.0f},
  {0.0f, 1.0f, 0.5f},
  {0.0f, 1.0f, 1.0f},
  {0.0f, 0.5f, 1.0f},
  {0.0f, 0.0f, 1.0f},
  {0.5f, 0.0f, 1.0f},
  {1.0f, 0.0f, 1.0f},
  {1.0f, 0.0f, 0.5f}
};

Font::Font(Game *G, const std::string& path) : G(G) {
  m_texture = content::Texture::TextureLoader::load(*G, content::Image::ImageFormats::PNG, path,
      PixelFormat::RGBA)->texture;
  std::ifstream source(path + ".fdf", std::ios_base::binary | std::ios_base::ate);
  if (source.good()) {
    int size = (int)source.tellg() - 1; // Last byte is font's height
    source.seekg(0, std::ios_base::beg);
    //widths = new uint8[size];
    texPos = new CharTexPos[95];
    uint8 widths[95];
    std::fill_n(widths, 95, 6);
    source.read((char*)widths, size);
    source.read((char*)&height, 1);

    int totalWidths = 0;
    for (uint8 i = 0; i < 95; ++i) {
      totalWidths += widths[i];
    }

    int left = 0;
    for (uint8 i = 0; i < 95; ++i) {
      texPos[i].width = widths[i];
      texPos[i].left = (float)left / totalWidths;
      left += widths[i];
      texPos[i].right = (float)left / totalWidths;
    }
  }

  G->R->renderers.font->registerFont(*this);
}

using Vertex = render::FontRenderer::TextBuffer::Vertex;

render::FontRendererTextBufferRef Font::createTextBuffer() const {
  return G->R->renderers.font->createTextBuffer();
}

render::FontRendererTextBufferRef Font::createTextBuffer(
    render::FontRendererTextBufferUsage usage) const {
  return G->R->renderers.font->createTextBuffer(usage);
}

#define eraseCurChar() vertCount -= 6;

void Font::updateTextBuffer(render::FontRendererTextBufferRef &buf, const std::string &text) const {
  uint vertCount = text.size()*6;
  std::unique_ptr<Vertex[]> verts(new Vertex[vertCount]);
  int16 c, w, line = 0, cx = 0, v = 0;
  float l, r;
  float cr = 1.0f, cg = 1.0f, cb = 1.0f, ca = 1.0f;
  for (size_t i = 0; i < text.size(); i++) {
    c = text[i];
    if (c == '\n') {
      eraseCurChar();
      line -= height;
      cx = 0;
      continue;
    }
    if (c == '\f' && i < text.length()) {
      /* \f can mean "format" */
      eraseCurChar();
      uint8 arg = text[++i];
      eraseCurChar();
      if (arg >= '0' && arg <= '9') {
        uint8 index = arg - '0';
        cr = ColorTable[index].r; cg = ColorTable[index].g; cb = ColorTable[index].b;
      } else if (arg >= 'a' && arg <= 'f') {
        uint8 index = (arg - 'a') + 10;
        cr = ColorTable[index].r; cg = ColorTable[index].g; cb = ColorTable[index].b;
      } else if (arg == 't') { // 't'ransparency
        uint8 arg2 = text[++i];
        eraseCurChar();
        if (arg2 >= 'a' && arg2 <= 'z')
          ca = (float)(arg2 - 'a')/('z'-'a');
      }
      continue;
    }
    if (c < ' ' || c > '~') {
      eraseCurChar();
      continue;
    }
    w = texPos[c - ' '].width;
    l = texPos[c - ' '].left;
    r = texPos[c - ' '].right;
    verts[v+0] = { cx, line, l, 1.0, cr, cg, cb, ca };
    verts[v+1] = { cx+w, line+height, r, 0.0, cr, cg, cb, ca };
    verts[v+2] = { cx, line+height, l, 0.0, cr, cg, cb, ca };
    verts[v+3] = { cx, line, l, 1.0, cr, cg, cb, ca };
    verts[v+4] = { cx+w, line, r, 1.0, cr, cg, cb, ca };
    verts[v+5] = { cx+w, line+height, r, 0.0, cr, cg, cb, ca };
    v += 6;
    cx += w;
  }
  G->R->renderers.font->updateTextBuffer(buf, verts.get(), vertCount);
}

void Font::draw(const render::FontRendererTextBufferRef &buf, const mat4 &matrix) const {
  G->R->renderers.font->render(*this, buf, matrix);
}

Font::Size Font::getSize(const std::string &text) const {
  uint8 c; int x = 0, y = height, cx = 0;
  for (uint i=0; i < text.size(); i++) {
    c = text[i];
    if (c == '\n') {
      y += height;
      if (cx > x)
        x = cx;
      cx = 0;
      continue;
    }
    if (c == '\f' && i < text.length()) { /* 167 = '§' */
      uint8 arg = text[++i];
      if (arg == 't') {
        i++;
      }
      continue;
    }
    x += texPos[c - ' '].width;
  }
  return Size { x, y };
}

int Font::getHeight() const {
  return height;
}

Font::~Font() {
  if (texPos)
    delete[] texPos;

   G->R->renderers.font->unregisterFont(*this);
}

}
}
