#ifndef DIGGLER_PARTICLES_HPP
#define DIGGLER_PARTICLES_HPP

#include <vector>

#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>

namespace diggler {

namespace render {
class ParticlesRenderer;
}
class Game;

struct Particle {
  // Keep it a POD struct
  glm::vec3 pos, vel, accel;
  glm::vec4 color;
  float size, decay;
};

class ParticleEmitter {
  friend class render::ParticlesRenderer;
  uintptr_t rendererData;

  Game *G;

  size_t count, maxCount;
  std::vector<Particle> particles;
public:
  struct ParticleRenderData {
    float x, y, z, r, g, b, a, s;
  };

  Particle pTemplate;
  glm::vec3 pos;

  glm::vec3 posAmpl, velAmpl;
  float decayAmpl;

  ParticleEmitter(Game*);
  ~ParticleEmitter();

  void setMaxCount(decltype(maxCount));
  decltype(maxCount) getMaxCount() const {
    return maxCount;
  }

  void emit(Particle&);
  void update(double delta);
};

}

#endif /* DIGGLER_PARTICLES_HPP */
