#ifndef DIGGLER_GAME_HPP
#define DIGGLER_GAME_HPP

#include <memory>

#include "ui/FontManager.hpp"
#include "Universe.hpp"
#include "PlayerList.hpp"
#include "LocalPlayer.hpp"
#include "render/gl/ProgramManager.hpp"

using std::shared_ptr;

namespace Diggler {

namespace Content {
class AssetManager;
class ModManager;
class Registry;
}

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
class Config;
class GameWindow;
class KeyBinds;
class Server;

class Game final {
private:
  template<typename T>
  using ptr = std::unique_ptr<T>;

public:
  // Shared
  Config *C;
  double Time; uint64 TimeMs;
  Net::Host H;
  Universe *U;
  PlayerList players;
  Content::Registry *CR;
  ptr<Content::AssetManager> AM;
  ptr<Content::ModManager> MM;
  Scripting::Lua::State *LS;

  // Server
  Server *S;

  // Client
  GameWindow *GW;
  UI::Manager *UIM;
  LocalPlayer *LP;
  Render::gl::ProgramManager *PM;
  Render::Renderer *R;
  ptr<UI::FontManager> FM;
  struct RenderProperties {
    bool bloom, wavingLiquids;
    float fogStart, fogEnd;
  } *RP;
  Audio *A;
  Net::Peer *NS;
  KeyBinds *KB;
  int PlayerPosUpdateFreq;

  Game();
  void init();
  void initClient();
  void initServer();
  void finalize();
  void finalizeClient();
  void finalizeServer();

  void updateTime(double time);
  ~Game();
};

}

#endif /* DIGGLER_GAME_HPP */
