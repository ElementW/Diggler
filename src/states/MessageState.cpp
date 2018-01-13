#include "MessageState.hpp"

#include <glm/gtc/matrix_transform.hpp>

#include "../Audio.hpp"
#include "../Game.hpp"
#include "../GlobalProperties.hpp"
#include "../Sound.hpp"
#include "../render/Renderer.hpp"
#include "../ui/Manager.hpp"
#include "../ui/Text.hpp"

namespace diggler {
namespace states {

MessageState::MessageState(GameWindow *W, const std::string &msg, const std::string &submsg)
  : W(W), msg(msg), subMsg(submsg), txtMsg(nullptr), txtSubMsg(nullptr) {
  glfwSetInputMode(*W, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

MessageState::~MessageState() {
}

void MessageState::onStart() {
  txtMsg = W->G->UIM->add<ui::Text>(msg, 2, 2);
  txtSubMsg = W->G->UIM->add<ui::Text>(subMsg);
  updateViewport();
  if (GlobalProperties::IsSoundEnabled) {
    W->G->A->playSound("click-quiet");
  }
}

void MessageState::updateViewport() {
  ui::Text::Size txtMsgSize = txtMsg->getSize();
  txtMsg->setPos(W->getW()/2 - txtMsgSize.x/2, W->getH()/2);
  
  ui::Text::Size txtSubMsgSize = txtSubMsg->getSize();
  txtSubMsg->setPos(W->getW()/2 - txtSubMsgSize.x / 2, W->getH()/2 - txtSubMsgSize.y);
}

void MessageState::onResize(int w, int h) {
  updateViewport();
}

void MessageState::onLogicTick() {}

void MessageState::onFrameTick() {
  W->G->R->beginFrame();
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  W->G->UIM->render();

  W->G->R->endFrame();
}

}
}
