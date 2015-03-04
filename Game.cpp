#include "Game.hpp"
#include "GlobalProperties.hpp"
#include "ChunkChangeHelper.hpp"
#include "Audio.hpp"
#include "KeyBinds.hpp"

namespace Diggler {

Game::Game() : players(this), CCH(nullptr), GW(nullptr), LP(nullptr), PM(nullptr) {
	SC = std::make_shared<Superchunk>(this);
	
	if (GlobalProperties::IsClient) {
		PM = new ProgramManager;
		LP = new LocalPlayer(this);
		RP = new RenderProperties;
		A = new Audio(this);
		KB = new KeyBinds();
	}
	if (GlobalProperties::IsServer) {
		CCH = new ChunkChangeHelper();
	}
}

Game::~Game() {
	if (GlobalProperties::IsClient) {
		delete PM; delete LP;
		delete RP; delete A;
		delete KB;
	}
	if (GlobalProperties::IsServer) {
		delete CCH;
	}
}

}