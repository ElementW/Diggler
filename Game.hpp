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

class Audio;

class Game {
public:
	// Shared
	shared_ptr<Superchunk> SC;
	PlayerList players;
	double Time;
	Net::Host H;
	
	// Server
	class Server *S;
	class ChunkChangeHelper *CCH;
	int Port = 61425;
	
	// Client
	class GameWindow *GW;
	UI::Manager *UIM;
	LocalPlayer *LP;
	ProgramManager *PM;
	Font *F;
	struct RenderProperties {
		bool bloom;
	} *RP;
	Audio *A;
	Net::Peer NS; // Net Server
	
	Game();
	~Game();
};

}

#endif