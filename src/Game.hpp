#ifndef DIGGLER_GAME_HPP
#define DIGGLER_GAME_HPP

#include <memory>

#include "Universe.hpp"
#include "PlayerList.hpp"

using std::shared_ptr;

namespace diggler {

namespace content {
class AssetManager;
class ModManager;
class Registry;
}

namespace gfx {
class Device;
}

namespace render {
class Renderer;
namespace gl {
class ProgramManager;
}
}

namespace scripting {
namespace lua {
class State;
}
}

namespace ui {
class FontManager;
class Manager;
}

class Audio;
class Config;
class GameWindow;
class KeyBinds;
class LocalPlayer;
class Server;

class Game final {
private:
  template<typename T>
  using ptr = std::unique_ptr<T>;

public:
  // Shared
  Config *C;
  double Time; uint64 TimeMs;
  net::Host H;
  Universe *U;
  PlayerList players;
  content::Registry *CR;
  ptr<content::AssetManager> AM;
  ptr<content::ModManager> MM;
  scripting::lua::State *LS;

  // Server
  Server *S;

  // Client
  ptr<gfx::Device> GD;
  GameWindow *GW;
  ui::Manager *UIM;
  LocalPlayer *LP;
  render::gl::ProgramManager *PM;
  render::Renderer *R;
  ptr<ui::FontManager> FM;
  struct RenderProperties {
    bool bloom, wavingLiquids;
    float fogStart, fogEnd;
  } *RP;
  Audio *A;
  net::Peer *NS;
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
