#ifndef UI_ELEMENT_HPP
#define UI_ELEMENT_HPP
#include "../Platform.hpp"
#include <glm/glm.hpp>

namespace Diggler {

class Game;

namespace UI {

class Manager;

class Element {
	friend Manager;

public:
	struct Point {
		int x, y;
	};
	struct Area {
		int x, y, w, h;
		bool isIn(int x, int y);
		inline bool isIn(const Point &p) { return isIn(p.x, p.y); }
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
	bool m_isCursorOver, m_hasFocus, m_isVisible;

protected:
	Area m_area;
	const Game *G;
	class Manager *M;
	const glm::mat4 *PM;

public:
	enum class MouseButton {
		Unknown,
		Left,
		Middle,
		Right
	};

	Element(Manager*);
	virtual ~Element();

	const Area& getArea() const { return m_area; }
	virtual void onAreaChange(const Area &newArea);
	virtual void onAreaChanged();
	void setArea(const Area &area) { onAreaChange(area); m_area = area; onAreaChanged(); }

	bool focusPassesThrough() { return false; }
	bool isFocused() const { return m_hasFocus; }
	virtual void onFocus();
	virtual void onFocusLost();

	bool cursorPassesThrough() { return false; }
	bool isCursorOver() const { return m_isCursorOver; }
	virtual void onMouseDown(int x, int y, MouseButton button);
	virtual void onMouseUp(int x, int y, MouseButton button);
	virtual void onMouseScroll(double x, double y);
	virtual void onCursorMove(int x, int y);
	virtual void onCursorEnter(int x, int y);
	virtual void onCursorLeave(int x, int y);

	void setVisible(bool v) { m_isVisible = v; }
	bool isVisible() const { return m_isVisible; }
	
	// Passing nullptr will reset matrix to default
	void setMatrix(const glm::mat4*);
	virtual void onMatrixChange();

	virtual void render() = 0;
};

}
}

#endif
