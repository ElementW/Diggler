#ifndef MESSAGE_STATE_HPP
#define MESSAGE_STATE_HPP
#include "State.hpp"
#include "GameWindow.hpp"
#include "ui/Text.hpp"

namespace Diggler {

class MessageState : public State {
private:
	GameWindow *W;
	std::string msg, subMsg;
	UI::Text *txtMsg, *txtSubMsg;

	void setupUI();

public:
	MessageState(GameWindow *W, const std::string &msg, const std::string &submsg = "");
	~MessageState();
	
	//void onMouseButton(int key, int action, int mods);
	//void onCursorPos(double x, double y);
	void onResize(int w, int h);
	void run();
	
	void updateViewport();
};

}

#endif