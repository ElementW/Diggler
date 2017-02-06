#ifndef TEXT_HPP
#define TEXT_HPP

#include <string>

#include "Font.hpp"
#include "Element.hpp"
#include "../render/FontRendererFwd.hpp"

namespace Diggler {
namespace UI {

class Text : public Element {
private:
  int m_scaleX, m_scaleY;
  Render::FontRendererTextBufferRef m_textBuf;
  std::string m_fontName;
  glm::mat4 m_matrix;
  std::string m_text;
  
  Text(const Text&) = delete;
  Text& operator=(const Text&) = delete;

public:
  struct Size { int x, y; };

  Text(Manager*, const std::string &text = "", int scaleX = 1, int scaleY = 1);
  Text(Text&&) = default;
  Text& operator=(Text&&) = default;
  ~Text();

  void setText(const std::string &text);
  std::string getText() const;
  void setFont(const std::string &name);
  void setPos(int x, int y);
  void setScale(int scaleX, int scaleY);
  Size getSize() const;
  void render();
  void render(const glm::mat4 &matrix) const;
  void onAreaChanged();
  void onMatrixChange();
  void update();
  void updateMatrix();
  void updateText();
};

}
}

#endif
