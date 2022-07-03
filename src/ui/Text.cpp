#include "Text.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "../Game.hpp"
#include "../GameWindow.hpp"
#include "../render/FontRenderer.hpp"
#include "FontManager.hpp"
#include "Manager.hpp"

namespace diggler {
namespace ui {

Text::Text(Manager *M) :
  Element(M),
  m_scaleX(1),
  m_scaleY(1),
  m_textBufUsage(render::FontRendererTextBufferUsage::Default) {
  updateMatrix();
}

Text::Text(Manager *M, const std::string &text, int scaleX, int scaleY) :
  Element(M),
  m_scaleX(scaleX),
  m_scaleY(scaleY),
  m_textBufUsage(render::FontRendererTextBufferUsage::Default),
  m_text(text) {
  updateText();
  updateMatrix();
}

Text::~Text() {
}

void Text::setUpdateFrequencyHint(render::FontRendererTextBufferUsage usage) {
  if (usage != m_textBufUsage) {
    m_textBufUsage = usage;
    m_textBuf.reset();
  }
}

void Text::setText(const std::string &text) {
  m_text = text;
  updateText();
}

std::string Text::getText() const {
  return m_text;
}

void Text::setFont(const std::string &name) {
  m_fontName = name;
  updateText();
}

void Text::setScale(int scaleX, int scaleY) {
  m_scaleX = scaleX;
  m_scaleY = scaleY;
  updateMatrix();
}

void Text::updateMatrix() {
  m_matrix = glm::scale(mat4(1.f), vec3(m_scaleX, m_scaleY, 1.f));
}

void Text::updateText() {
  if (!m_textBuf) {
    m_textBuf = M->G->FM->getFont(m_fontName)->createTextBuffer(m_textBufUsage);
  }
  M->G->FM->getFont(m_fontName)->updateTextBuffer(m_textBuf, m_text);
}

void Text::render(const mat4 &matrix) const {
  if (m_textBuf) {
    M->G->FM->getFont(m_fontName)->draw(m_textBuf, matrix * m_matrix);
  }
}

Text::Size Text::getSize() const {
  Font::Size s = M->G->FM->getFont(m_fontName)->getSize(m_text);
  return Size { s.x*m_scaleX, s.y*m_scaleY };
}

}
}
