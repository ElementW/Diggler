#ifndef STATE_HPP
#define STATE_HPP
#include "Platform.hpp"

namespace Diggler {

class GameWindow;

class State {
private:
  friend GameWindow;

  double m_runTime, m_deltaTime;

protected:
  inline double runTime() const {
    return m_runTime;
  }
  inline double deltaTime() const {
    return m_deltaTime;
  }

public:
  virtual ~State() {}

  // Lifecycle
  virtual void onCreate() {}
  virtual void onStart() {}
  // virtual void onResume() {}
  virtual void onLogicTick() = 0;
  virtual void onFrameTick() = 0;
  // virtual void onPause() {}
  virtual void onStop() {}
  virtual void onDestroy() {}

  // Input
  virtual void onMouseButton(int key, int action, int mods) {}
  virtual void onCursorPos(double x, double y) {}
  virtual void onMouseScroll(double x, double y) {}
  virtual void onKey(int key, int scancode, int action, int mods) {}
  virtual void onChar(char32 unichar) {}
  virtual void onResize(int w, int h) {}
};

}

#endif
