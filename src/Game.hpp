#ifndef GAME_HPP
#define GAME_HPP
#include <memory>
#include "ui/FontManager.hpp"
#include "Universe.hpp"
#include "PlayerList.hpp"
#include "LocalPlayer.hpp"
#include "ProgramManager.hpp"

using std::shared_ptr;

namespace Diggler {

namespace UI {
class Manager;
}

class Audio;
class ContentRegistry;
class Config;
class GameWindow;
class KeyBinds;
class Server;

class Game {
public:
	// Shared
	PlayerList players;
	double Time; uint64 TimeMs;
	Net::Host H;
	Universe *U;
	ContentRegistry *CR;
	
	// Server
	Server *S;
	
	// Client
	Config *C;
	GameWindow *GW;
	UI::Manager *UIM;
	LocalPlayer *LP;
	ProgramManager *PM;
	UI::FontManager FM;
	struct RenderProperties {
		bool bloom, wavingLiquids;
		float fogStart, fogEnd;
	} *RP;
	Audio *A;
	Net::Peer NS;
	KeyBinds *KB;
	int PlayerPosUpdateFreq;
	
	Game();
	void init();
	void uninitGL();
	void updateTime(double time);
	~Game();
};

}

#endif