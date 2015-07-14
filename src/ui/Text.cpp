#include "Text.hpp"
#include "../GameWindow.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace Diggler {
namespace UI {

Text::Text(Manager *M, Font *font, const std::string &text, int scaleX, int scaleY)
	: Element(M), m_scaleX(scaleX), m_scaleY(scaleY), m_font(font), m_text(text) {
	update();
}

void Text::setText(const std::string &text) {
	m_text = text;
	update();
}

std::string Text::getText() const {
	return m_text;
}

void Text::setFont(Font *font) {
	m_font = font;
	update();
}

void Text::setPos(int x, int y) {
	setArea(Area {x, y, 0, 0});
}

void Text::setScale(int scaleX, int scaleY) {
	m_scaleX = scaleX;
	m_scaleY = scaleY;
	updateMatrix();
}

void Text::onMatrixChange() {
	updateMatrix();
}

void Text::onAreaChanged() {
	updateMatrix();
}

void Text::update() {
	updateText();
	updateMatrix();
}

void Text::updateMatrix() {
	m_matrix = glm::translate(*PM, glm::vec3(m_area.x, m_area.y, 0.f));
	m_matrix = glm::scale(m_matrix, glm::vec3(m_scaleX, m_scaleY, 1.f));
}

void Text::updateText() {
	m_elementCount = m_font->updateVBO(m_vbo, m_text);
}

void Text::render() {
	m_font->draw(m_vbo, m_elementCount, m_matrix);
}

void Text::render(const glm::mat4 &matrix) const {
	m_font->draw(m_vbo, m_elementCount, matrix);
}

Text::Size Text::getSize() const {
	Font::Size s = m_font->getSize(m_text);
	return Size { s.x*m_scaleX, s.y*m_scaleY };
}

}
}