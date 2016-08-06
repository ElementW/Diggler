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

namespace Render {
class Renderer;
}

namespace Scripting {
namespace Lua {
class State;
}
}

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
  double Time; uint64 TimeMs;
  Net::Host H;
  Universe *U;
  PlayerList players;
  ContentRegistry *CR;
  Scripting::Lua::State *LS;

  // Server
  Server *S;

  // Client
  Config *C;
  GameWindow *GW;
  UI::Manager *UIM;
  LocalPlayer *LP;
  ProgramManager *PM;
  Render::Renderer *R;
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
  void updateTime(double time);
  ~Game();
};

}

#endif
