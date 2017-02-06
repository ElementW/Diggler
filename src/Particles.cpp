#include "Particles.hpp"
#include "Platform.hpp"

#include "Game.hpp"
#include "render/Renderer.hpp"

namespace Diggler {

ParticleEmitter::ParticleEmitter(Game *G) :
  G(G) {
  G->R->renderers.particles->registerEmitter(*this);
}

ParticleEmitter::~ParticleEmitter() {
  G->R->renderers.particles->unregisterEmitter(*this);
}

void ParticleEmitter::setMaxCount(decltype(maxCount) count) {
  particles.reserve(count);
  for (decltype(maxCount) i = maxCount; i < count; ++i) {
    particles[i].decay = -1;
  }
  maxCount = count;
}

void ParticleEmitter::emit(Particle &p) {
  p.pos = this->pos + glm::vec3(posAmpl.x*(FastRandF()*2-1), posAmpl.y*(FastRandF()*2-1), posAmpl.z*(FastRandF()*2-1));
  p.vel = pTemplate.vel + glm::vec3(velAmpl.x*(FastRandF()*2-1), velAmpl.y*(FastRandF()*2-1), velAmpl.z*(FastRandF()*2-1));
  p.accel = pTemplate.accel;
  p.color = pTemplate.color;
  p.size = pTemplate.size;
  p.decay = pTemplate.decay + (FastRandF()*2-1)*decayAmpl;
}

void ParticleEmitter::update(double delta) {
  std::unique_ptr<ParticleRenderData[]> data(new ParticleRenderData[maxCount]);
  float deltaF = delta;
  for (decltype(maxCount) i = 0; i < maxCount; ++i) {
    Particle &p = particles[i];
    p.vel += p.accel * deltaF;
    p.pos += p.vel * deltaF;
    p.decay -= deltaF;
    if (p.decay < 0)
      emit(p);
    data[i] = { p.pos.x, p.pos.y, p.pos.z, p.color.r, p.color.g, p.color.b, p.color.a, p.size };
  }
  G->R->renderers.particles->updateParticleData(*this, data.get(), maxCount);
}

}
