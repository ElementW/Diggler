#include "Element.hpp"
#include "Manager.hpp"

namespace Diggler {
namespace UI {

bool Element::Area::isIn(int x, int y) {
  return x >= this->x && x <= (this->x + this->w) &&
         y >= this->y && y <= (this->y + this->h);
}


Element::Element(Manager *M) :
  m_isCursorOver(false),
  m_hasFocus(false),
  m_isVisible(true),
  m_isManual(false),
  m_area({0, 0, 0, 0}), M(M) {
  PM = M->PM;
  G = M->G;
}

Element::~Element() {
}

void Element::onAreaChange(const Area&) {
}

void Element::onAreaChanged() {
}

void Element::onFocus() {
}

void Element::onFocusLost() {
}

void Element::onMouseDown(int, int, MouseButton) {
}

void Element::onMouseUp(int, int, MouseButton) {
}

void Element::onMouseScroll(double x, double y) {
}

void Element::onCursorMove(int, int) {
}

void Element::onCursorEnter(int, int) {
}

void Element::onCursorLeave(int, int) {
}

void Element::onMatrixChange() {
}

void Element::setMatrix(const glm::mat4 *m) {
  PM = m ? m : M->PM;
  onMatrixChange();
}

}
}
