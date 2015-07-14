#include "Element.hpp"
#include "Manager.hpp"

namespace Diggler {
namespace UI {

Element::Element(Manager *M) : m_isVisible(true), m_area({0, 0, 0, 0}), M(M) {
	PM = M->PM;
	G = M->G;
}

Element::~Element() {
}

void Element::setMatrix(const glm::mat4 *m) {
	PM = m ? m : M->PM;
	onMatrixChange();
}

void Element::onAreaChange(const Area&) {
}

void Element::onAreaChanged() {
}

void Element::onMatrixChange() {
}

void Element::onMouseDown(int, int, MouseButton) {
}

void Element::onMouseUp(int, int, MouseButton) {
}

void Element::onMouseMove(int, int) {
}

void Element::onMouseEnter(int, int) {
}

void Element::onMouseLeave(int, int) {
}

}
}