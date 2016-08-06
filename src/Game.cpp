#include "Game.hpp"

#include "Audio.hpp"
#include "content/Registry.hpp"
#include "GlobalProperties.hpp"
#include "KeyBinds.hpp"
#include "render/gl/Renderer.hpp"
#include "scripting/lua/State.hpp"

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
  CR = new ContentRegistry;
  LS = new Scripting::Lua::State(this);
  if (GlobalProperties::IsClient) {
    PM = new ProgramManager(*this);
    LP = new LocalPlayer(this);
    RP = new RenderProperties; { // TODO move somewhere else?
      RP->bloom = true;
      RP->wavingLiquids = !true;
      RP->fogStart = 16;
      RP->fogEnd = 24;
    }
    R = new Render::gl::GLRenderer(this);
    A = new Audio(*this);
    KB = new KeyBinds;
    PlayerPosUpdateFreq = 4;
  }
  if (GlobalProperties::IsServer) {
  }
}

Game::~Game() {
  delete CR;
  if (GlobalProperties::IsClient) {
    delete KB;
    delete A;
    delete R;
    delete RP;
    delete LP;
    delete PM;
  }
  if (GlobalProperties::IsServer) {
  }
}

void Game::updateTime(double time) {
  Time = time;
  TimeMs = (int64)(time * 1000);
}

}
