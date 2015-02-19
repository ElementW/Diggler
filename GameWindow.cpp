#include "GameWindow.hpp"
#include <al.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Game.hpp"
#include "GlobalProperties.hpp"
#include "GLFWHandler.hpp"
#include "GameState.hpp"
#include "MessageState.hpp"
#include "Audio.hpp"

namespace Diggler {

int GameWindow::InstanceCount = 0;
bool GameWindow::IsGlewInited = false;

GameWindow::GameWindow() {
	if (InstanceCount++ == 0) {
		int glfwStatus = glfwInit();
		if (glfwStatus != GL_TRUE) {
			getDebugStream() << "GLFW init failed: " << glfwStatus << std::endl;
			std::terminate();
		}
		std::cout << "GLFW " << glfwGetVersionString() << std::endl;
	}
	
	GLFWHandler::getInstance().setWindow(this, m_window);
	
	m_w = 640; m_h = 480;
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
	glfwWindowHint(GLFW_SAMPLES, 0); // Gimme aliasing everywhere
	//glfwWindowHint(GLFW_STENCIL_BITS, 8);
	m_window = glfwCreateWindow(m_w, m_h, "Diggler", nullptr, nullptr);
	if (!m_window) {
		glfwTerminate();
		std::terminate();
	}
	glfwSetFramebufferSizeCallback(m_window, GLFWHandler::resize);
	glfwSetCursorPosCallback(m_window, GLFWHandler::cursorPos);
	glfwSetKeyCallback(m_window, GLFWHandler::key);
	glfwSetMouseButtonCallback(m_window, GLFWHandler::mouseButton);
	glfwSetCharCallback(m_window, GLFWHandler::unichar);
	glfwMakeContextCurrent(m_window);
	glfwSwapInterval(1);
	/*GLint bits;
	glGetIntegerv(GL_STENCIL_BITS, &bits);
	getDebugStream() << bits << " stencil bits" << std::endl;*/
	
	if (!IsGlewInited) {
		GLenum glewStatus = glewInit();
		if (glewStatus != GLEW_OK) {
			getDebugStream() << "GLEW init failed: " << glewStatus << std::endl;
			std::terminate();
		}
		IsGlewInited = true;
		std::cout << "GLEW " << glewGetString(GLEW_VERSION) << std::endl;
	}
	
	if (InstanceCount == 1) { // If we're the first instance
		const uint8 *GL_version = glGetString(GL_VERSION);
		//const uint8 *GL_vendor = glGetString(GL_VENDOR);
		const uint8 *GL_renderer = glGetString(GL_RENDERER);
		std::cout << "GL " << GL_version << " / " << GL_renderer << std::endl;
	}
	
	UIM.setProjMat(glm::ortho(0.0f, (float)m_w, 0.0f, (float)m_h));
	
	G = new Game;
	UIM.setup(G);
	G->GW = this;
	G->UIM = &UIM;
	G->A->loadSoundAssets();
	
	G->F = new Font(G, getAssetPath("04b08.png"));
	
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

GameWindow::~GameWindow() {
	delete G->F;
	delete G;
	
	glfwDestroyWindow(m_window);
	
	if (--InstanceCount == 0) {
		glfwTerminate();
	}
}

void GameWindow::cbChar(char32 unichar) {
	m_currentState->onChar(unichar);
}

void GameWindow::cbKey(int key, int scancode, int action, int mods) {
	m_currentState->onKey(key, scancode, action, mods);
}

void GameWindow::cbMouseButton(int key, int action, int mods) {
	m_currentState->onMouseButton(key, action, mods);
}

void GameWindow::cbCursorPos(double x, double y) {
	m_currentState->onCursorPos(x, y);
}

void GameWindow::cbMouseScroll(double x, double y) {
	m_currentState->onMouseScroll(x, y);
}

void GameWindow::cbResize(int w, int h) {
	m_w = w; m_h = h;
	UIM.setProjMat(glm::ortho(0.0f, (float)w, 0.0f, (float)h));
	m_currentState->onResize(w, h);
}

void GameWindow::setNextState(const shared_ptr<State> next) {
	m_nextState = next;
}

void GameWindow::run() {
	while (m_nextState != nullptr && !glfwWindowShouldClose(m_window)) {
		m_currentState = m_nextState;
		m_nextState = nullptr;
		UIM.clear();
		m_currentState->run();
	}
}

void GameWindow::showMessage(const std::string &msg, const std::string &submsg) {
	setNextState(std::make_shared<MessageState>(this, msg, submsg));
}

}