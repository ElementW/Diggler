#ifndef DIGGLER_STATES_MESSAGE_STATE_HPP
#define DIGGLER_STATES_MESSAGE_STATE_HPP
#include "State.hpp"

#include <memory>
#include <string>

#include "../GameWindow.hpp"
#include "../ui/Text.hpp"

namespace diggler {
namespace states {

class MessageState : public State {
private:
  GameWindow *W;
  std::string msg, subMsg;
  std::shared_ptr<ui::Text> txtMsg, txtSubMsg;

  void setupUI();

public:
  MessageState(GameWindow *W, const std::string &msg, const std::string &submsg = "");
  ~MessageState() override;

  void onStart() override;
  void onLogicTick() override;
  void onFrameTick() override;

  //void onMouseButton(int key, int action, int mods);
  //void onCursorPos(double x, double y);
  void onResize(int w, int h) override;
  
  void updateViewport();
};

}
}

#endif /* DIGGLER_STATES_MESSAGE_STATE_HPP */
