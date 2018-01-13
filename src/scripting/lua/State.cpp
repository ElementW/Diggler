#include "State.hpp"

#include <sstream>
#include <stdexcept>

extern "C" {
#include <lauxlib.h>
#include <lualib.h>
#include <luajit.h>
}

namespace diggler {
namespace scripting {
namespace lua {

static int wrap_exceptions(lua_State *L, lua_CFunction f) {
  try {
    return f(L);
  } catch (const char *s) {
    lua_pushstring(L, s);
  } catch (const std::exception &e) {
    lua_pushstring(L, e.what());
  } catch (...) {
    lua_pushliteral(L, "caught (...)");
  }
  return lua_error(L);
}

static std::string intpad(int i, int digits) {
  std::string ret(digits, ' ');
  bool neg = i < 0;
  if (neg) {
    i = -i;
  }
  for (int d = digits - 1; d > 0; --d) {
    if (i > 0) {
      ret[d] = '0' + (i % 10);
      i /= 10;
    } else if (neg) {
      ret[d] = '-';
      break;
    }
  }
  return ret;
}

static std::string stackdump(lua_State* L) {
  std::ostringstream oss;
  int i, top = lua_gettop(L);
  oss << "Lua: " << top << " stack entries" << std::endl;
  for (i = 1; i <= top; i++) {
    int t = lua_type(L, i);
    switch (t) {
      case LUA_TSTRING:
        oss << intpad(i, 3) << " string: '" << lua_tostring(L, i) << "'" << std::endl;
        break;
      case LUA_TBOOLEAN:
        oss << intpad(i, 3) << " bool: " << (lua_toboolean(L, i) ? "true" : "false") << std::endl;
        break;
      case LUA_TNUMBER:
        oss << intpad(i, 3) << " number: " << lua_tonumber(L, i) << std::endl;
        break;
      default:
        oss << intpad(i, 3) << ' ' << lua_typename(L, t) << std::endl;
        break;
    }
  }
  return oss.str();
}

State::State(Game *G) :
  G(G),
  state(nullptr) {
}

State::~State() {
  if (state != nullptr) {
    finalize();
  }
}

void State::initialize() {
  state = luaL_newstate();

  lua_pushlightuserdata(state, (void*) wrap_exceptions);
  luaJIT_setmode(state, -1, LUAJIT_MODE_WRAPCFUNC | LUAJIT_MODE_ON);
  lua_pop(state, 1);

  luaL_openlibs(state);

  lua_pushstring(state, "DigglerGameInstance");
  lua_pushlightuserdata(state, G);
  lua_settable(state, LUA_REGISTRYINDEX);

  lua_pushstring(state, "DigglerErrorHandler");
  lua_pushcfunction(state, handleLuaError);
  lua_settable(state, LUA_REGISTRYINDEX);


  lua_newtable(state);
  lua_pushlstring(state, reinterpret_cast<const char*>(&G), sizeof(G));
  lua_setfield(state, -2, "gameInstancePtrStr");
  lua_setglobal(state, "digglerNative");
}

void State::finalize() {
  lua_close(state);
  state = nullptr;
}

std::string State::traceback() {
  std::cout << stackdump(state);
  if (!lua_isstring(state, 1))
    return "";
  lua_getglobal(state, "debug");
  if (!lua_istable(state, -1)) {
    lua_pop(state, 1);
    return "";
  }
  lua_getfield(state, -1, "traceback");
  if (!lua_isfunction(state, -1)) {
    lua_pop(state, 2);
    return "";
  }
  lua_pushvalue(state, 1);
  lua_pushinteger(state, 2);
  lua_call(state, 2, 1);
  const char *err = lua_tostring(state, -1);
  return err ? err : "<no traceback>";
}

int State::handleLuaError(lua_State *state) {
  lua_getfield(state, LUA_REGISTRYINDEX, "DigglerGameInstance");
  Game *G = static_cast<Game*>(lua_touserdata(state, -1));

  G->LS->error = G->LS->traceback();
  return 0;
}

void State::setGameLuaRuntimePath(const std::string &path) {
  lua_getglobal(state, "digglerNative");
  lua_pushstring(state, path.c_str());
  lua_setfield(state, -2, "gameLuaRuntimePath");
  lua_pop(state, 1);
}

void State::dofile(const char *path) {
  int luaRet;
  lua_getfield(state, LUA_REGISTRYINDEX, "DigglerErrorHandler");
  if ((luaRet = luaL_loadfile(state, path))) {
    lua_remove(state, -2); // pop error handler
    if (hasError()) {
      std::string err(std::move(error));
      throw std::runtime_error(err);
    } else {
      throw std::runtime_error(traceback());
    }
  }
  if ((luaRet = lua_pcall(state, 0, 0, -2))) {
    lua_remove(state, -2); // pop error handler
    if (hasError()) {
      std::string err(std::move(error));
      throw std::runtime_error(err);
    } else {
      throw std::runtime_error(traceback());
    }
  }
  lua_pop(state, 1);
}

void State::dostring(const char *code) {
  int luaRet;
  lua_getfield(state, LUA_REGISTRYINDEX, "DigglerErrorHandler");
  if ((luaRet = luaL_loadstring(state, code))) {
    lua_remove(state, -2); // pop error handler
    if (hasError()) {
      std::string err(std::move(error));
      throw std::runtime_error(err);
    } else {
      throw std::runtime_error(traceback());
    }
  }
  if ((luaRet = lua_pcall(state, 0, 0, -2))) {
    lua_remove(state, -2); // pop error handler
    if (hasError()) {
      std::string err(std::move(error));
      throw std::runtime_error(err);
    } else {
      throw std::runtime_error(traceback());
    }
  }
  lua_pop(state, 1);
}

}
}
}
