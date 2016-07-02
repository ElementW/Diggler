#ifndef PLAYER_HPP
#define PLAYER_HPP
#include "Platform.hpp"
#include <list>
#include <glm/glm.hpp>
#include <epoxy/gl.h>
#include "network/Network.hpp"
#include "World.hpp"

namespace Diggler {

class Program;
class VBO;
class Game;
class Texture;

using PlayerGameID = uint32;

class Player {
protected:
  static struct Renderer {
    const Program *prog;
    GLint att_coord,
        uni_mvp,
        uni_unicolor,
        uni_fogStart,
        uni_fogEnd;
    VBO *vbo;
  } R;
  double m_lastPosTime;
  glm::vec3 m_predictPos;
  
  Player(const Player&) = delete;
  Player& operator=(const Player&) = delete;

public:
  enum class Direction : uint8 {
    North,	// To +Z
    East,	// To +X
    South,	// To -X
    West	// To -Z
  } direction;
  enum class DeathReason : uint8 {
    None,
    Lava,
    Shock,
    Fall,
    Explosion,
    Void
  } deathReason;
  Game *G;
  WorldRef W;
  glm::vec3 position, lastPosition, velocity, accel;
  float angle; double toolUseTime;
  std::string name;
  uint32 id;
  bool isAlive;
  Net::Peer P;
  std::list<ChunkRef> pendingChunks;

  Player(Game *G = nullptr);
  Player(Player&&);
  Player& operator=(Player&&);
  ~Player();

  void setPosVel(const glm::vec3 &pos, const glm::vec3 &vel, const glm::vec3 &acc = glm::vec3());
  void update(const float &delta);
  void render(const glm::mat4 &transform) const;
  void setDead(bool, DeathReason = DeathReason::None, bool send = false);
};

}

#endif
