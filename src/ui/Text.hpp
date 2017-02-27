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
  Render::FontRendererTextBufferUsage m_textBufUsage;
  Render::FontRendererTextBufferRef m_textBuf;
  std::string m_fontName;
  glm::mat4 m_matrix;
  std::string m_text;
  
  Text(const Text&) = delete;
  Text& operator=(const Text&) = delete;

  void updateMatrix();
  void updateText();

public:
  struct Size { int x, y; };

  Text(Manager*);
  Text(Manager*, const std::string &text, int scaleX = 1, int scaleY = 1);
  Text(Text&&) = default;
  Text& operator=(Text&&) = default;
  ~Text();

  /**
   * Hints if frequent updates should be expected for this Text object
   */
  void setUpdateFrequencyHint(Render::FontRendererTextBufferUsage);

  /**
   * @param text Text to display
   */
  void setText(const std::string &text);

  /**
   * @return Displayed text
   */
  std::string getText() const;

  /**
   * Sets the font to use when displaying text
   * @param name Font name
   */
  void setFont(const std::string &name);
  void setScale(int scaleX, int scaleY);
  Size getSize() const;
  void render(const glm::mat4&) const override;
  using Element::render;
};

}
}

#endif
