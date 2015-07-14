#include "UITestState.hpp"
#include "GameWindow.hpp"

namespace Diggler {

UITestState::UITestState(GameWindow *W) : W(W) {
}

UITestState::~UITestState() {
}

void UITestState::run() {
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