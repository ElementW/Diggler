#ifndef GAME_STATE_HPP
#define GAME_STATE_HPP
#include "State.hpp"
#include <thread>
#include <map>
#include <glm/detail/type_vec2.hpp>
#include <glm/detail/type_vec.hpp>
#include "GameWindow.hpp"
#include "VBO.hpp"
#include "Blocks.hpp"
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

	struct Bloom {
		bool enable;
		int scale;
		struct Extractor {
			FBO *fbo;
			const Program *prog;
			GLuint att_coord, att_texcoord, uni_mvp;
		} extractor;
		struct Renderer {
			FBO *fbo;
			const Program *prog;
			GLuint att_coord, att_texcoord, uni_pixshift, uni_mvp;
		} renderer;
		Bloom(Game&);
		~Bloom();
	} bloom;

	VBO *m_3dRenderVBO;
	struct Coord2DTex { int x, y; uint8 u, v; };
	Clouds *m_clouds;
	Skybox *m_sky;

	struct CrossHair {
		Texture *tex;
		glm::mat4 mat;
	} m_crossHair;

	struct {
		VBO vbo;
		const Program *program;
		GLuint att_coord, uni_unicolor, uni_mvp;
	} m_highlightBox;

	struct BuilderGun {
		Texture *tex;
		std::map<BlockType, Texture*> blockTexs;
		int index;
		bool deconstruct;
		BlockType currentBlock;
		Texture  *currentBlockTex;
		BuilderGun();
		~BuilderGun();
	} m_builderGun;

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
		struct {
			glm::mat4 mat;
			glm::vec4 color;
		} headerBg;

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
	void lockMouse();
	void unlockMouse();

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

	void sendMsg(Net::OutMessage &msg, Net::Tfer mode, Net::Channels chan = Net::Channels::Base);
};

}

#endif