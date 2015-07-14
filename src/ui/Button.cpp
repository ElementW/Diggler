#include "Button.hpp"
#include "../Game.hpp"
#include "Text.hpp"

namespace Diggler {
namespace UI {

Button::Button(Manager *M, const Area &area) : Element(M) {
	m_area = area;
	
	color = {0.4, 0.4, 0.4, 1.0};
	m_displayedColor = &color;
}

void Button::onMouseDown(int x, int y, MouseButton button) {
	
}

void Button::onMouseUp(int x, int y, MouseButton button) {
	
}

void Button::onMatrixChange() {
	
}

void Button::onMouseEnter(int x, int y) {
	
}

void Button::onMouseLeave(int x, int y) {
	
}

void Button::render() {
	
}

void Button::update() {
	Coord quad[6] = {
		{m_area.x, m_area.y, 0, 0},
		{m_area.x+m_area.w, m_area.y, 1, 0},
		{m_area.x, m_area.y+m_area.h, 0, 1},
		
		{m_area.x+m_area.w, m_area.y+m_area.h, 1, 1},
		{m_area.x+m_area.w, m_area.y, 1, 0},
		{m_area.x, m_area.y+m_area.h, 0, 1}
	};
	m_vbo.setData(quad, 6*sizeof(Coord));
}

}
}