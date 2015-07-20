#include "UITestState.hpp"
#include "GameWindow.hpp"

namespace Diggler {

UITestState::UITestState(GameWindow *W) : GW(W) {
}

UITestState::~UITestState() {
}

void UITestState::run() {
	while (!glfwWindowShouldClose(*GW)) { // Infinite loop \o/
		glClearColor(sin(glfwGetTime()*4)*0.5+0.5, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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