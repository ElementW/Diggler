#ifndef UI_TEST_STATE_HPP
#define UI_TEST_STATE_HPP
#include "State.hpp"

namespace Diggler {

class GameWindow;
namespace UI {
class Text;
}

class UITestState : public State {
private:
	GameWindow *GW;
	UI::Text *txt;

public:
	UITestState(GameWindow*);
	~UITestState();
	
	void onMouseButton(int key, int action, int mods);
	void onCursorPos(double x, double y);
	void onResize(int w, int h);
	void onMouseScroll(double x, double y);
	void run();
	
	void updateViewport();
};

}

#endif