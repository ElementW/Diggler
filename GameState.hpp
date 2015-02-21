#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP
#include "State.hpp"
#include <thread>
#include <glm/detail/type_vec2.hpp>
#include <glm/detail/type_vec.hpp>
#include "GameWindow.hpp"
#include "VBO.hpp"
#include "network/Network.hpp"

namespace Diggler {

class Skybox;
class KeyBindings;
class Font;
class Program;
class Clouds;
class FBO;
class Game;
class Chatbox;
namespace UI {
class Text;
}

class GameState : public State {
private:
	GameWindow *W;
	Game *G;

	std::string m_serverHost;
	int m_serverPort;

	glm::vec3 m_lookat;
	glm::vec2 m_angles;

	FBO *m_3dFbo;
	const Program *m_3dFboRenderer;
	GLuint m_3dFboRenderer_coord, m_3dFboRenderer_texcoord, m_3dFboRenderer_mvp;

	bool enableExtractor;
	FBO *m_extractorFbo;
	const Program *m_bloomExtractorRenderer;
	GLuint m_bloomExtractorRenderer_coord, m_bloomExtractorRenderer_texcoord, m_bloomExtractorRenderer_mvp;
	static const int BloomScale;
	FBO *m_bloomFbo;
	const Program *m_bloomRenderer;
	GLuint m_bloomRenderer_coord, m_bloomRenderer_texcoord, m_bloomRenderer_mvp, m_bloomRenderer_pixshift;

	VBO *m_3dRenderVBO;
	struct Coord2DTex { int x, y; uint8 u, v; };
	Clouds *m_clouds;
	Skybox *m_sky;

	KeyBindings *m_keybinds;

	Chatbox *m_chatBox;

	bool m_mouseLocked; int cX, cY;
	glm::vec3 angles, lookat;

	std::thread m_networkThread;

	Net::InMessage m_msg;
	float nextNetUpdate;

	struct ConnectingUI {
		UI::Text *Connecting;
		UI::Text *Dot;
	};

	bool isEscapeToggled = false,
		 showDebugInfo = false;
	struct {
		UI::Text *Ore;
		UI::Text *Loot;
		UI::Text *Weight;
		UI::Text *TeamOre;
		UI::Text *RedCash;
		UI::Text *BlueCash;

		UI::Text *FPS;
		UI::Text *Altitude;
		int lastAltitude;

		UI::Text *DebugInfo;

		class EscMenu *EM;
	} UI;

	void setupUI();

public:
	GameState(GameWindow *W, const std::string &servHost, int servPort);
	~GameState();

	void onMouseButton(int key, int action, int mods);
	void onCursorPos(double x, double y);
	void onMouseScroll(double x, double y);
	void onKey(int key, int scancode, int action, int mods);
	void onChar(char32 unichar);
	void onResize(int w, int h);
	void run();

	void updateViewport();

	bool connectLoop();
	bool teamSelectLoop();
	bool classSelectLoop();
	void gameLoop();

	void renderDeathScreen();
	void updateUI();
	void drawUI();
	bool processNetwork();

	void sendMsg(Net::OutMessage &msg, Net::Tfer mode);
};

}

#endif