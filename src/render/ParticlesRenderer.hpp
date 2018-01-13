#ifndef DIGGLER_RENDER_PARTICLES_RENDERER_HPP
#define DIGGLER_RENDER_PARTICLES_RENDERER_HPP

#include "RenderParams.hpp"
#include "../Particles.hpp"

namespace diggler {
namespace render {

class ParticlesRenderer {
protected:
  inline uintptr_t getRendererData(const ParticleEmitter &pe) const {
    return pe.rendererData;
  }
  inline void setRendererData(ParticleEmitter &pe, uintptr_t data) const {
    pe.rendererData = data;
  }

public:
  virtual ~ParticlesRenderer() = 0;

  virtual void registerEmitter(ParticleEmitter&) = 0;
  virtual void updateParticleData(ParticleEmitter&, ParticleEmitter::ParticleRenderData *data,
    size_t count) = 0;
  virtual void unregisterEmitter(ParticleEmitter&) = 0;

  virtual void render(RenderParams&) = 0;
};

inline ParticlesRenderer::~ParticlesRenderer() {}

}
}

#endif /* DIGGLER_RENDER_PARTICLES_RENDERER_HPP */
