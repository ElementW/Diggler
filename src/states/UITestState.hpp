#ifndef DIGGLER_STATES_UI_TEST_STATE_HPP
#define DIGGLER_STATES_UI_TEST_STATE_HPP
#include "State.hpp"

namespace diggler {

class GameWindow;
namespace ui {
class Text;
}

namespace states {

class UITestState : public State {
private:
  GameWindow *GW;
  ui::Text *txt;

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
}

#endif /* DIGGLER_STATES_UI_TEST_STATE_HPP */
