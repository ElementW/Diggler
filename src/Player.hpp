#ifndef DIGGLER_PLAYER_HPP
#define DIGGLER_PLAYER_HPP

#include <list>
#include <memory>

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>

#include "platform/PreprocUtils.hpp"
#include "platform/Types.hpp"

#include "network/Network.hpp"
#include "render/gl/OpenGL.hpp"
#include "Chunk.hpp"
#include "World.hpp"

namespace diggler {

namespace render {
class PlayerRenderer;
namespace gl {
class Program;
class VBO;
}
}
class Game;
class Texture;

using PlayerGameID = uint32;

class Player {
protected:
  friend class render::PlayerRenderer;
  uintptr_t rendererData;

  static struct Renderer {
    const render::gl::Program *prog;
    GLint att_coord,
        uni_mvp,
        uni_unicolor,
        uni_fogStart,
        uni_fogEnd;
    std::unique_ptr<render::gl::VBO> vbo;
  } R;
  double m_lastPosTime;
  glm::vec3 m_predictPos;

public:
  enum class Direction : uint8 {
    North, // To +Z
    East,  // To +X
    South, // To -X
    West   // To -Z
  } direction;

  Game *G;
  WorldRef W;
  glm::vec3 position, lastPosition, velocity, accel;
  float angle; double toolUseTime;
  std::string name;
  using SessionID = uint32;
  SessionID sessId;
  bool isAlive;
  net::Peer *peer;
  std::list<ChunkRef> pendingChunks;

  Player(Game *G = nullptr);
  nocopy(Player);
  defaultmove(Player);

  void setPosVel(const glm::vec3 &pos, const glm::vec3 &vel, const glm::vec3 &acc = glm::vec3());
  void update(const float &delta);
  void render(const glm::mat4 &transform) const;
  void setDead(bool);
};

}

#endif /* DIGGLER_PLAYER_HPP */
