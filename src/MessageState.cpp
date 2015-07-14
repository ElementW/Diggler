#include "MessageState.hpp"
#include "_.hpp"
#include "ui/Text.hpp"
#include "Game.hpp"
#include "GlobalProperties.hpp"
#include "Sound.hpp"
#include "Audio.hpp"
#include <glm/gtc/matrix_transform.hpp>

namespace Diggler {

MessageState::MessageState(GameWindow *W, const std::string &msg, const std::string &submsg)
	: W(W), msg(msg), subMsg(submsg), txtMsg(nullptr), txtSubMsg(nullptr) {
	glfwSetInputMode(*W, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
}

MessageState::~MessageState() {
}

void MessageState::setupUI() {
	txtMsg = W->G->UIM->add<UI::Text>(W->G->F, msg, 2, 2);
	txtSubMsg = W->G->UIM->add<UI::Text>(W->G->F, subMsg);
	updateViewport();
}

void MessageState::updateViewport() {
	glViewport(0, 0, W->getW(), W->getH());
	
	UI::Text::Size txtMsgSize = txtMsg->getSize();
	txtMsg->setPos(W->getW()/2 - txtMsgSize.x/2, W->getH()/2);
	
	UI::Text::Size txtSubMsgSize = txtSubMsg->getSize();
	txtSubMsg->setPos(W->getW()/2 - txtSubMsgSize.x / 2, W->getH()/2 - txtSubMsgSize.y);
}

void MessageState::onResize(int w, int h) {
	updateViewport();
}

void MessageState::run() {
	setupUI();
	if (GlobalProperties::IsSoundEnabled) {
		W->G->A->playSound("click-quiet");
	}
	while (!glfwWindowShouldClose(*W)) {
		glClearColor(0.0, 0.0, 0.0, 1.0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		W->G->UIM->render();
		
		glfwSwapBuffers(*W);
		glfwPollEvents();
	}
}

}