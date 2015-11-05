#ifndef UI_BUTTON_HPP
#define UI_BUTTON_HPP
#include "Element.hpp"
#include <memory>
#include "../VBO.hpp"

namespace Diggler {
namespace UI {

class Text;

class Button : public Element {
private:
	std::unique_ptr<Text> m_text;
	std::string m_label;
	Color *m_displayedColor;
	void update();

public:
	Color color, colorHover, colorPress;

	Button(Manager*, const Area &area, const std::string &label = "<?>");

	void setLabel(const std::string &label);
	std::string getLabel() const;

	void onMatrixChange();
	void onAreaChange(const Area &newArea);
	void onMouseDown(int x, int y, MouseButton button);
	void onMouseUp(int x, int y, MouseButton button);
	void onCursorEnter(int x, int y);
	void onCursorLeave(int x, int y);

	void render();
};

}
}

#endif