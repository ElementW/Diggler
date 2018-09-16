#ifndef GAME_WINDOW_HPP
#define GAME_WINDOW_HPP

#include <memory>

#include "Audio.hpp"  // for alc.h

#include "render/gl/OpenGL.hpp"
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

#include "StateMachine.hpp"
#include "Platform.hpp"
#include "platform/Types.hpp"

namespace diggler {

class Game;

namespace ui {
class Manager;
}

class GameWindow : public StateMachine {
private:
  static int InstanceCount;

  GLFWwindow *m_window;
  int m_w, m_h;

  std::unique_ptr<states::State> m_currentState, m_nextState;

public:
  ui::Manager *UIM;

  Game *G;

  GameWindow(Game*);
  ~GameWindow() override;

  operator GLFWwindow&() const { return *m_window; }
  operator GLFWwindow*() const { return m_window; }

  inline int getW() const { return m_w; }
  inline int getH() const { return m_h; }

  bool shouldClose() const;

  void setVisible(bool);
  bool isVisible() const;

  void cbMouseButton(int key, int action, int mods);
  void cbCursorPos(double x, double y);
  void cbMouseScroll(double x, double y);
  void cbKey(int key, int scancode, int action, int mods);
  void cbChar(char32 unichar);
  void cbResize(int w, int h);

  void updateViewport();

  states::State& state() const override;
  void setNextState(std::unique_ptr<states::State> &&next) override;

  void run();

  void showMessage(const std::string &msg, const std::string &submsg = "");
};

}

#endif
