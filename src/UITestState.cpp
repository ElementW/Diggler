#include "UITestState.hpp"
#include "ui/Button.hpp"
#include "Game.hpp"
#include "GameWindow.hpp"

namespace Diggler {

UITestState::UITestState(GameWindow *W) : GW(W) {
}

UITestState::~UITestState() {
}

void UITestState::run() {
  std::shared_ptr<UI::Button>
    b1 = GW->G->UIM->add<UI::Button>(UI::Element::Area { 0, 0, 320, 60 }),
    b2 = GW->G->UIM->add<UI::Button>(UI::Element::Area { 320, 60, 320, 60 });

  while (!glfwWindowShouldClose(*GW)) { // Infinite loop \o/
    glClearColor(sin(glfwGetTime()*4)*0.1+0.5, 0, 0, 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    b1->setArea(UI::Element::Area{ 0, 0, 300+20*sin(glfwGetTime()*4), 60 });
    b2->setArea(UI::Element::Area{ 300+20*sin(glfwGetTime()*4), 60, 320, 60 });

    GW->G->UIM->render();

    glfwSwapBuffers(*GW);
    glfwPollEvents();
  }
}

void UITestState::updateViewport() {
}

void UITestState::onMouseScroll(double x, double y) {
}

void UITestState::onResize(int w, int h) {
}

void UITestState::onMouseButton(int key, int action, int mods) {
}

void UITestState::onCursorPos(double x, double y) {
}

}
