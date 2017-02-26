#ifndef DIGGLER_CONNECTING_STATE_HPP
#define DIGGLER_CONNECTING_STATE_HPP

#include "State.hpp"
#include "GameWindow.hpp"
#include "ui/Text.hpp"

namespace Diggler {

namespace UI {
class Text;
}

class ConnectingState : public State {
private:
  GameWindow *W;

  std::shared_ptr<UI::Text> txtConnecting, txtDot;

  std::string m_serverHost;
  int m_serverPort;

  void setupUI();

public:
  ConnectingState(GameWindow *W, const std::string &servHost, int servPort);
  ~ConnectingState();

  //void onMouseButton(int key, int action, int mods);
  //void onCursorPos(double x, double y);
  void onResize(int w, int h);
  void run();

  void updateViewport();
};

}

#endif /* DIGGLER_CONNECTING_STATE_HPP */
