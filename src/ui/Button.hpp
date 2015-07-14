#ifndef UI_BUTTON_HPP
#define UI_BUTTON_HPP
#include "Element.hpp"
#include "../VBO.hpp"

namespace Diggler {
namespace UI {

class Button : public Element {
private:
	class Text *m_text;
	VBO m_vbo;
	Color *m_displayedColor;
	void update();

public:
	Color color, colorHover, colorPress;
	
	Button(Manager*, const Area& area);
	
	void render();
	void onMatrixChange();
	void onMouseDown(int x, int y, MouseButton button);
	void onMouseUp(int x, int y, MouseButton button);
	void onMouseEnter(int x, int y);
	void onMouseLeave(int x, int y);
};

}
}

#endif