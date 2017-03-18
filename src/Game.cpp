#include "Game.hpp"

#include "Audio.hpp"
#include "content/AssetManager.hpp"
#include "content/ModManager.hpp"
#include "content/Registry.hpp"
#include "GlobalProperties.hpp"
#include "KeyBinds.hpp"
#include "LocalPlayer.hpp"
#include "render/gl/ProgramManager.hpp"
#include "render/gl/Renderer.hpp"
#include "scripting/lua/State.hpp"
#include "ui/FontManager.hpp"

namespace Diggler {

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
  AM = std::make_unique<Content::AssetManager>(this);
  MM = std::make_unique<Content::ModManager>(this);
  LS = new Scripting::Lua::State(this);
  if (GlobalProperties::IsClient) {
    initClient();
  }
  if (GlobalProperties::IsServer) {
    initServer();
  }
}

void Game::initClient() {
  PM = new Render::gl::ProgramManager(*this);
  LP = new LocalPlayer(this);
  RP = new RenderProperties; { // TODO move somewhere else?
    RP->bloom = true;
    RP->wavingLiquids = !true;
    RP->fogStart = 16;
    RP->fogEnd = 24;
  }
  R = new Render::gl::GLRenderer(this);
  CR = new Content::Registry(*this);
  FM = std::make_unique<UI::FontManager>(*this);
  A = new Audio(*this);
  KB = new KeyBinds;
  LP = new LocalPlayer(this);
  PlayerPosUpdateFreq = 4;
}

void Game::initServer() {
  CR = new Content::Registry(*this);
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
