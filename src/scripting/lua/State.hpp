#ifndef DIGGLER_SCRIPTING_LUA_STATE_HPP
#define DIGGLER_SCRIPTING_LUA_STATE_HPP

extern "C" {
#include <lua.h>
}

#include "../../Game.hpp"

namespace diggler {
namespace scripting {
namespace lua {

class State {
private:
  Game *G;

  static int handleLuaError(lua_State*);
  std::string error;
  inline bool hasError() const {
    return error.size() > 0;
  }
  inline void clearError() {
    error.clear();
  }

public:
  lua_State *state;

  State(Game*);
  ~State();

  void initialize();
  void finalize();

  std::string traceback();

  void setGameLuaRuntimePath(const std::string &path);

  void dofile(const char *path);
  inline void dofile(const std::string &path) {
    dofile(path.c_str());
  }

  void dostring(const char *code);
  inline void dostring(const std::string &code) {
    dostring(code.c_str());
  }
};

}
}
}

#endif /* DIGGLER_SCRIPTING_LUA_STATE_HPP */
