#include "GLFWHandler.hpp"
#include "GameWindow.hpp"

namespace Diggler {

void GLFWHandler::mouseButtonImpl(GLFWwindow *window, int key, int action, int mods) {
	win->cbMouseButton(key, action, mods);
}

void GLFWHandler::cursorPosImpl(GLFWwindow *window, double x, double y) {
	win->cbCursorPos(x, y);
}

void GLFWHandler::mouseScrollImpl(GLFWwindow *window, double x, double y) {
	win->cbMouseScroll(x, y);
}

void GLFWHandler::keyImpl(GLFWwindow *window, int key, int scancode, int action, int mods) {
	win->cbKey(key, scancode, action, mods);
}

void GLFWHandler::unicharImpl(GLFWwindow *window, unsigned int unichar) {
	win->cbChar(static_cast<char32>(unichar));
}

void GLFWHandler::resizeImpl(GLFWwindow *window, int w, int h) {
	win->cbResize(w, h);
}


}