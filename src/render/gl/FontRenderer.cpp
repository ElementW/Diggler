#include "FontRenderer.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "../../Game.hpp"

namespace Diggler {
namespace Render {
namespace gl {

GLFontRenderer::GLTextBuffer::GLTextBuffer(const GLFontRenderer &parent) :
  parent(parent) {
  { VAO::Config cfg = vao.configure();
    cfg.vertexAttrib(vbo, parent.att_coord, 2, GL_SHORT, sizeof(Vertex), 0);
    cfg.vertexAttrib(vbo, parent.att_texcoord, 2, GL_FLOAT, sizeof(Vertex), offsetof(Vertex, tx));
    cfg.vertexAttrib(vbo, parent.att_color, 4, GL_FLOAT, sizeof(Vertex), offsetof(Vertex, r));
    cfg.commit();
  }
}

GLFontRenderer::GLTextBuffer::~GLTextBuffer() {
}

GLFontRenderer::GLFontRenderer(Game *G) :
  G(G) {
  prog = G->PM->getProgram(PM_2D | PM_TEXTURED | PM_COLORED);
  att_coord = prog->att("coord");
  att_texcoord = prog->att("texcoord");
  att_color = prog->att("color");
  uni_mvp = prog->uni("mvp");
}

GLFontRenderer::~GLFontRenderer() {
}

GLFontRenderer::TextBufferRef GLFontRenderer::createTextBuffer() {
  return std::make_unique<GLTextBuffer>(*this);
}

void GLFontRenderer::registerFont(UI::Font&) {

}

void GLFontRenderer::unregisterFont(UI::Font&) {

}

void GLFontRenderer::updateTextBuffer(TextBufferRef &buf, const TextBuffer::Vertex *vertices,
    uint vertexCount) {
  GLTextBuffer &glbuf = *reinterpret_cast<GLTextBuffer*>(buf.get());
  glbuf.vbo.setDataKeepSize(vertices, vertexCount);
  glbuf.vertexCount = static_cast<GLsizei>(vertexCount);
}

void GLFontRenderer::render(const UI::Font &font, const TextBufferRef &buf,
    const glm::mat4 &matrix) {
  const GLTextBuffer &glbuf = *reinterpret_cast<const GLTextBuffer*>(buf.get());
  prog->bind();
  font.texture()->bind();
  glbuf.vao.bind();
  glUniformMatrix4fv(uni_mvp, 1, GL_FALSE, glm::value_ptr(matrix));
  glDrawArrays(GL_TRIANGLES, 0, glbuf.vertexCount);
  glbuf.vao.unbind();
}

}
}
}
