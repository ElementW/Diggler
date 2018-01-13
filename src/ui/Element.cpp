#include "Element.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "Manager.hpp"

namespace diggler {
namespace ui {

bool Element::Area::isIn(int x, int y) const {
  return x >= this->x && x < (this->x + this->w) &&
         y >= this->y && y < (this->y + this->h);
}


Element::Element(Manager *M) :
  m_isCursorOver(false),
  m_hasFocus(false),
  m_isVisible(true),
  m_manualRender(false),
  m_manualInput(false),
  m_renderArea({0, 0, 0, 0}),
  m_inputArea({0, 0, 0, 0}),
  M(M) {
  G = M->G;
}

Element::~Element() {
}

void Element::setManual(bool render, bool input) {
  m_manualRender = render;
  m_manualInput = input;
}

void Element::onRenderAreaChange(const Area&) {
}

void Element::onRenderAreaChanged() {
}

void Element::onInputAreaChange(const Area&) {
}

void Element::onInputAreaChanged() {
}

void Element::onFocus() {
}

void Element::onFocusLost() {
}

void Element::onMouseDown(int, int, MouseButton) {
}

void Element::onMouseUp(int, int, MouseButton) {
}

void Element::onMouseScroll(double, double) {
}

void Element::onCursorMove(int, int) {
}

void Element::onCursorEnter(int, int) {
}

void Element::onCursorLeave(int, int) {
}

void Element::render() const {
  render(glm::translate(*M->PM, glm::vec3(m_renderArea.x, m_renderArea.y, 0)));
}

}
}
