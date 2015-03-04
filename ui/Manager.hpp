#ifndef UI_MANAGER_HPP
#define UI_MANAGER_HPP
#include <list>
#include "Element.hpp"
#include "../_.hpp"

namespace Diggler {

class VBO;
class GameWindow;
class Texture;

namespace UI {

class Manager {
private:
	_<VBO> m_rectVbo;
	std::list<_<Element>> m_elements;
	glm::mat4 m_projMatrix, m_projMat1, m_projMat1V;
	
	friend GameWindow;
	void setProjMat(const glm::mat4&);

public:
	const glm::mat4 *PM, *PM1;
	Game *G;

	Manager();
	void setup(Game*);

	void onMouseButton(int key, int action, int mods);
	void onCursorPos(double x, double y);
	void onMouseScroll(double x, double y);
	void onKey(int key, int scancode, int action, int mods);
	void onChar(char32 unichar);
	void onResize(int w, int h);
	
	template<class T, typename... Args> T* add(Args&&... args) {
		T *obj = new T(this, std::forward<Args>(args)...);
		m_elements.emplace_back(reinterpret_cast<Element*>(obj));
		return obj;
	}
	void add(Element*);
	template<class T, typename... Args> T* create(Args&&... args) {
		return new T(this, std::forward<Args>(args)...);
	}
	void remove(Element*);
	void clear();
	
	// Utility
	
	//void drawRect(const Element::Area&, const glm::vec3 &color) const;
	void drawTexRect(const Element::Area&, const Texture&) const;
	void drawFullTexV(const Texture&);
	
	// Render
	
	void render();
};

}
}

#endif