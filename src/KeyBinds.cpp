#include "KeyBinds.hpp"
#include <cstdio>
#include <GLFW/glfw3.h>

using std::fopen;
using std::fwrite;
using std::fread;
using std::fclose;

namespace Diggler {

KeyBinds::KeyBinds() {
  loadDefaults();
}

void KeyBinds::loadDefaults() {
  forward		= GLFW_KEY_W;
  backward	= GLFW_KEY_S;
  left		= GLFW_KEY_A;
  right		= GLFW_KEY_D;
  jump		= GLFW_KEY_SPACE;
  chat		= GLFW_KEY_T;
  chatTeam	= GLFW_KEY_Y;
  gameMenu	= GLFW_KEY_ESCAPE;
  playerList	= GLFW_KEY_TAB;
}

void KeyBinds::load(const std::string &path) {
  FILE *f = fopen(path.c_str(), "r");
  if (!f)
    return;
  int key;
  auto readBind = [&f, &key]() mutable -> int { fread(&key, sizeof(int), 1, f); return key; };
  forward		= readBind();
  backward	= readBind();
  left		= readBind();
  right		= readBind();
  jump		= readBind();
  chat		= readBind();
  chatTeam	= readBind();
  gameMenu	= readBind();
  playerList	= readBind();
  fclose(f);
}

void KeyBinds::save(const std::string &path) const {
  FILE *f = fopen(path.c_str(), "w");
  if (!f)
    return;
  auto writeBind = [&f](int key) mutable { fwrite(&key, sizeof(int), 1, f); };
  writeBind(forward);
  writeBind(backward);
  writeBind(left);
  writeBind(right);
  writeBind(jump);
  writeBind(chat);
  writeBind(chatTeam);
  writeBind(gameMenu);
  writeBind(playerList);
  fclose(f);
}

}