#ifndef LOCAL_PLAYER_HPP
#define LOCAL_PLAYER_HPP
#include "Camera.hpp"
#include "Platform.hpp"
#include "ui/Text.hpp"
#include "Player.hpp"
#include "World.hpp"

namespace Diggler {

class Game;

class LocalPlayer : public Player {
private:
  bool goingForward, goingBackward, goingLeft, goingRight;
  bool hasGravity, hasNoclip, onGround, onRoad;

public:
  float health;
  // Fixes
  double lastJumpTime = 0.0;
  double deathTime = 0.0;
  bool deathSent = false, deathShown = false;

public:
  Camera camera;
  glm::vec3 extents, eyesPos;
  glm::ivec3 aabbmin, aabbmax;

  LocalPlayer(Game *G);

  inline glm::mat4 getPVMatrix() { return camera.getPVMatrix(); }
  void lookAt(const glm::vec3 &at);
  inline void setProjection(const glm::mat4 &p) { camera.setProjection(p); }
  void update(float delta);
  void render(const glm::mat4 &transform) const;
  void forceCameraUpdate();
  void goForward(bool enable);
  void goBackward(bool enable);
  void goLeft(bool enable);
  void goRight(bool enable);
  void setHasGravity(bool fall);
  void setHasNoclip(bool fly);
  void jump();
  void setDead(bool, DeathReason = DeathReason::None, bool send = false);
  bool raytracePointed(glm::ivec3 *pointed, glm::ivec3 *facing);
  bool raytracePointed(float range, glm::ivec3 *pointed, glm::ivec3 *facing);
};

}

#endif