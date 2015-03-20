#include "Game.hpp"
#include "GlobalProperties.hpp"
#include "ChunkChangeHelper.hpp"
#include "Audio.hpp"
#include "KeyBinds.hpp"

namespace Diggler {

Game::Game() : players(this), CCH(nullptr), GW(nullptr), LP(nullptr), PM(nullptr) {
	SC = std::make_shared<Superchunk>(this);
}

void Game::init() {
	if (GlobalProperties::IsClient) {
		PM = new ProgramManager(*this);
		LP = new LocalPlayer(this);
		RP = new RenderProperties; { // TODO move somewhere else?
			RP->bloom = true;
			RP->fogStart = 16;
			RP->fogEnd = 32;
		}
		A = new Audio(*this);
		KB = new KeyBinds;
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