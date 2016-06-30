#include "Button.hpp"
#include "../Game.hpp"
#include "Manager.hpp"
#include "Text.hpp"

namespace Diggler {
namespace UI {

Button::Button(Manager *M, const Area &area, const std::string &label) : Element(M) {
  m_area = area;
  
  color = {0.4, 0.4, 0.4, 1.0};
  m_displayedColor = &color;

  m_text.reset(M->create<Text>("", 2, 2));
  m_text->setMatrix(PM);
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

void Button::onMatrixChange() {
  m_text->onMatrixChange();
}

void Button::onAreaChange(const Area &newArea) {
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

void Button::render() {
  M->drawRect(m_area, glm::vec4(1, 1, 1, isCursorOver() ? 0.5 : 0.3));
  m_text->render();
}

void Button::update() {
  
}

}
}