#include "Game.hpp"
#include "Audio.hpp"
#include "content/Registry.hpp"
#include "GlobalProperties.hpp"
#include "KeyBinds.hpp"

namespace Diggler {

Game::Game() :
	U(nullptr),
	players(this),
	GW(nullptr),
	LP(nullptr),
	PM(nullptr),
	FM(*this) {
}

void Game::init() {
	U = new Universe(this);
	if (GlobalProperties::IsClient) {
		CR = new ContentRegistry;
		PM = new ProgramManager(*this);
		LP = new LocalPlayer(this);
		RP = new RenderProperties; { // TODO move somewhere else?
			RP->bloom = true;
			RP->wavingLiquids = !true;
			RP->fogStart = 16;
			RP->fogEnd = 24;
		}
		A = new Audio(*this);
		KB = new KeyBinds;
		PlayerPosUpdateFreq = 4;
	}
	if (GlobalProperties::IsServer) {
	}
}

void Game::uninitGL() {
	delete PM;
	delete RP;
}

Game::~Game() {
	if (GlobalProperties::IsClient) {
		delete A;
		delete CR;
		delete LP;
		delete KB;
	}
	if (GlobalProperties::IsServer) {
	}
}

void Game::updateTime(double time) {
	Time = time;
	TimeMs = (int64)(time * 1000);
}

}