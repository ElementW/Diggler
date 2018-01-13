#include "Game.hpp"

#include "content/AssetManager.hpp"
#include "content/ModManager.hpp"
#include "content/Registry.hpp"
#include "gfx/Device.hpp"
#include "render/gl/ProgramManager.hpp"
#include "render/gl/Renderer.hpp"
#include "scripting/lua/State.hpp"
#include "ui/FontManager.hpp"
#include "Audio.hpp"
#include "GlobalProperties.hpp"
#include "KeyBinds.hpp"
#include "LocalPlayer.hpp"

namespace diggler {

Game::Game() :
  C(nullptr),
  U(nullptr),
  players(this),
  CR(nullptr),
  LS(nullptr),

  S(nullptr),

  GW(nullptr),
  UIM(nullptr),
  LP(nullptr),
  PM(nullptr),
  R(nullptr),
  RP(nullptr),
  A(nullptr),
  NS(nullptr),
  KB(nullptr) {
}

void Game::init() {
  AM = std::make_unique<content::AssetManager>(this);
  MM = std::make_unique<content::ModManager>(this);
  LS = new scripting::lua::State(this);
  if (GlobalProperties::IsClient) {
    initClient();
  }
  if (GlobalProperties::IsServer) {
    initServer();
  }
}

void Game::initClient() {
  PM = new render::gl::ProgramManager(*this);
  LP = new LocalPlayer(this);
  RP = new RenderProperties; { // TODO move somewhere else?
    RP->bloom = true;
    RP->wavingLiquids = !true;
    RP->fogStart = 16;
    RP->fogEnd = 24;
  }
  R = new render::gl::GLRenderer(this);
  CR = new content::Registry(*this);
  FM = std::make_unique<ui::FontManager>(*this);
  A = new Audio(*this);
  KB = new KeyBinds;
  LP = new LocalPlayer(this);
  PlayerPosUpdateFreq = 4;
}

void Game::initServer() {
  CR = new content::Registry(*this);
}

void Game::finalize() {
  if (GlobalProperties::IsClient) {
    finalizeClient();
  }
  if (GlobalProperties::IsServer) {
    finalizeServer();
  }
  delete LS; LS = nullptr;
  MM.reset();
  AM.reset();
  delete CR; CR = nullptr;
}

void Game::finalizeClient() {
  delete LP; LP = nullptr;
  delete KB; KB = nullptr;
  delete A; A = nullptr;
  FM.reset();
  delete R; R = nullptr;
  delete RP; RP = nullptr;
  delete PM; PM = nullptr;
}

void Game::finalizeServer() {
}

Game::~Game() {
  finalize();
}

void Game::updateTime(double time) {
  Time = time;
  TimeMs = static_cast<uint64>(time * 1000);
}

}
