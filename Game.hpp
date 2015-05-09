#ifndef GAME_HPP
#define GAME_HPP
#include <memory>
#include "Superchunk.hpp"
#include "PlayerList.hpp"
#include "LocalPlayer.hpp"
#include "ProgramManager.hpp"

using std::shared_ptr;

namespace Diggler {

namespace UI {
class Manager;
}

class Game {
public:
	// Shared
	shared_ptr<Superchunk> SC;
	PlayerList players;
	double Time; uint64 TimeMs;
	Net::Host H;
	
	// Server
	class Server *S;
	class ChunkChangeHelper *CCH;
	
	// Client
	Blocks *B;
	class Config *C;
	class GameWindow *GW;
	UI::Manager *UIM;
	LocalPlayer *LP;
	ProgramManager *PM;
	Font *F;
	struct RenderProperties {
		bool bloom, wavingLiquids;
		float fogStart, fogEnd;
	} *RP;
	class Audio *A;
	Net::Peer NS;
	class KeyBinds *KB;
	int PlayerPosUpdateFreq;
	
	Game();
	void init();
	void updateTime(double time);
	~Game();
};

}

#endif