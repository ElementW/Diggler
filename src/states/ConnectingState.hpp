#ifndef DIGGLER_STATES_CONNECTING_STATE_HPP
#define DIGGLER_STATES_CONNECTING_STATE_HPP
#include "State.hpp"

#include <memory>
#include <string>
#include <thread>

#include "../GameWindow.hpp"
#include "../ui/Text.hpp"

namespace diggler {

namespace ui {
class Text;
}

namespace states {

class ConnectingState : public State {
private:
  GameWindow *W;

  std::shared_ptr<ui::Text> txtConnecting, txtDot;

  std::string m_serverHost;
  int m_serverPort;

  std::thread m_networkThread;
  bool finished, success;
  std::string m_infoStr;

  void setupUI();

public:
  ConnectingState(GameWindow *W, const std::string &servHost, int servPort);
  ~ConnectingState() override;

  void onStart() override;
  void onLogicTick() override;
  void onFrameTick() override;
  void onStop() override;

  //void onMouseButton(int key, int action, int mods);
  //void onCursorPos(double x, double y);
  void onResize(int w, int h) override;

  void updateViewport();
};

}
}

#endif /* DIGGLER_STATES_CONNECTING_STATE_HPP */
