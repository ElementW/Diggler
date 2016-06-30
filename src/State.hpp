#ifndef STATE_HPP
#define STATE_HPP
#include "Platform.hpp"

namespace Diggler {

class State {
public:
  virtual void onMouseButton(int key, int action, int mods) {}
  virtual void onCursorPos(double x, double y) {}
  virtual void onMouseScroll(double x, double y) {}
  virtual void onKey(int key, int scancode, int action, int mods) {}
  virtual void onChar(char32 unichar) {}
  virtual void onResize(int w, int h) {}
  virtual void run() = 0;
};

}

#endif