#ifndef UI_ELEMENT_HPP
#define UI_ELEMENT_HPP
#include "../Platform.hpp"
#include <glm/glm.hpp>

namespace Diggler {

class Game;

namespace UI {

class Element {
	friend class Manager;

public:
	struct Area {
		int x, y, w, h;
	};
	struct Coord {
		int x, y;
		uint8 r, g, b, a;
		float u, v;
	};
	struct Color {
		float r, g, b, a;
	};

private:
	bool m_hasFocus, m_isVisible;

protected:
	Area m_area;
	const Game *G;
	class Manager *M;
	const glm::mat4 *PM;

public:
	enum MouseButton {
		Left,
		Middle,
		Right
	};
	
	Element(Manager*);
	virtual ~Element() {}
	
	const Area& getArea() const { return m_area; }
	virtual void onAreaChange(const Area &newArea) {}
	virtual void onAreaChanged() {}
	void setArea(const Area &area) { onAreaChange(area); m_area = area; onAreaChanged(); }
	
	bool focusPassesThrough() { return false; }
	bool isFocused() const { return m_hasFocus; }
	void setIsFocused(bool focus) { m_hasFocus = focus; }
	
	void setVisible(bool v) { m_isVisible = v; }
	bool isVisible() const { return m_isVisible; }
	
	// Passing nullptr will reset matrix to default
	void setMatrix(const glm::mat4*);
	virtual void onMatrixChange() {}
	virtual void render() = 0;
	
	virtual void onMouseDown(int x, int y, MouseButton button) {}
	virtual void onMouseUp(int x, int y, MouseButton button) {}
	virtual void onMouseMove(int x, int y) {}
	virtual void onMouseEnter(int x, int y) {}
	virtual void onMouseLeave(int x, int y) {}
};

}
}

#endif