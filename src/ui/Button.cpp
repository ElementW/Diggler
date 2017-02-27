#include "Button.hpp"
#include "../Game.hpp"
#include "Manager.hpp"
#include "Text.hpp"

namespace Diggler {
namespace UI {

Button::Button(Manager *M, const Area &area, const std::string &label) : Element(M) {
  setArea(area);

  color = {0.4, 0.4, 0.4, 1.0};
  m_displayedColor = &color;

  m_text = M->addManual<Text>("", 2, 2);
  m_text->setArea(area);
  setLabel(label);
}

void Button::setLabel(const std::string &label) {
  m_label = label;
  m_text->setText(label);
}

std::string Button::getLabel() const {
  return m_label;
}

void Button::onRenderAreaChange(const Area &newArea) {
  m_text->setArea(newArea);
}

void Button::onMouseDown(int x, int y, MouseButton button) {
}

void Button::onMouseUp(int x, int y, MouseButton button) {
}

void Button::onCursorEnter(int x, int y) {
}

void Button::onCursorLeave(int x, int y) {
}

void Button::render(const glm::mat4 &matrix) const {
  M->drawRect(matrix, glm::vec4(1, 1, 1, isCursorOver() ? 0.5 : 0.3));
  m_text->render(matrix);
}

void Button::update() {
  
}

}
}
