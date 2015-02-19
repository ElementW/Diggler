#include "Element.hpp"
#include "Manager.hpp"

namespace Diggler {
namespace UI {

Element::Element(Manager *M) : m_isVisible(true), m_area({0, 0, 0, 0}), M(M) {
	PM = M->PM;
	G = M->G;
}

}
}