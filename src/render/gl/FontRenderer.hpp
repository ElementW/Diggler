#ifndef DIGGLER_RENDER_GL_FONT_RENDERER_HPP
#define DIGGLER_RENDER_GL_FONT_RENDERER_HPP

#include "../FontRenderer.hpp"

#include "Program.hpp"
#include "VAO.hpp"
#include "VBO.hpp"

namespace Diggler {

class Game;

namespace Render {
namespace gl {

class GLFontRenderer : public FontRenderer {
protected:
  Game *G;
  const Program *prog;
  GLint att_coord,
        att_texcoord,
        att_color,
        uni_mvp;

public:
  class GLTextBuffer : public TextBuffer {
  public:
    const GLFontRenderer &parent;
    VBO vbo;
    VAO vao;
    GLsizei vertexCount;
    FontRendererTextBufferUsage usage;

    GLTextBuffer(const GLFontRenderer&, FontRendererTextBufferUsage);
    ~GLTextBuffer() final override;
  };

  GLFontRenderer(Game*);
  ~GLFontRenderer();

  void registerFont(UI::Font&) final override;
  void unregisterFont(UI::Font&) final override;

  TextBufferRef createTextBuffer(FontRendererTextBufferUsage) final override;
  void updateTextBuffer(TextBufferRef&, const TextBuffer::Vertex *vertices,
      uint vertexCount) final override;

  void render(const UI::Font&, const TextBufferRef&, const glm::mat4&) final override;
};

}
}
}

#endif /* DIGGLER_RENDER_GL_FONT_RENDERER_HPP */
