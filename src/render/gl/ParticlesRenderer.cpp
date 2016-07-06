#include "ParticlesRenderer.hpp"

#include <glm/gtc/type_ptr.hpp>

#include "../../Game.hpp"

namespace Diggler {
namespace Render {
namespace gl {

using GLParticle = ParticleEmitter::ParticleRenderData;

GLParticlesRenderer::GLParticlesRenderer(Game *G) :
  G(G) {
  prog = G->PM->getProgram(PM_3D | PM_POINTSIZE | PM_COLORED | PM_FOG);
  att_coord = prog->att("coord");
  att_color = prog->att("color");
  //att_texcoord = prog->att("texcoord");
  att_pointSize = prog->att("pointSize");
  uni_mvp = prog->uni("mvp");
  uni_unicolor = prog->uni("unicolor");
  uni_fogStart = prog->uni("fogStart");
  uni_fogEnd = prog->uni("fogEnd");
}

GLParticlesRenderer::~GLParticlesRenderer() {
}

void GLParticlesRenderer::registerEmitter(ParticleEmitter *pe) {
  if (pe == nullptr) {
    return;
  }
  emitters.emplace_back();
  emitters.back().emitter = pe;
  setRendererData(pe, emitters.size() - 1);
}

void GLParticlesRenderer::updateParticleData(ParticleEmitter *pe,
  ParticleEmitter::ParticleRenderData *data, uint count) {
  if (pe == nullptr) {
    return;
  }
  uintptr_t idx = getRendererData(pe);
  EmitterEntry &entry = emitters.at(idx);
  entry.vbo.setDataKeepSize(data, count, GL_STREAM_DRAW);
}

void GLParticlesRenderer::unregisterEmitter(ParticleEmitter *pe) {
  if (pe == nullptr) {
    return;
  }
  uintptr_t idx = getRendererData(pe);
  emitters.erase(emitters.begin() + idx);
}

void GLParticlesRenderer::render(RenderParams &rp) {
  prog->bind();
  glEnableVertexAttribArray(att_coord);
  glEnableVertexAttribArray(att_color);
  glEnableVertexAttribArray(att_pointSize);
  glUniformMatrix4fv(uni_mvp, 1, GL_FALSE, glm::value_ptr(rp.transform));
  glUniform1f(uni_fogStart, G->RP->fogStart);
  glUniform1f(uni_fogEnd, G->RP->fogEnd);
  glUniform4f(uni_unicolor, 1.f, 1.f, 1.f, 1.f);

  for (EmitterEntry &entry : emitters) {
    entry.vbo.bind();
    glVertexAttribPointer(att_coord, 3, GL_FLOAT, GL_FALSE, sizeof(GLParticle), 0);
    glVertexAttribPointer(att_color, 4, GL_FLOAT, GL_FALSE, sizeof(GLParticle), (GLvoid*)(3*sizeof(float)));
    glVertexAttribPointer(att_pointSize, 1, GL_FLOAT, GL_FALSE, sizeof(GLParticle), (GLvoid*)(7*sizeof(float)));
    glDrawArrays(GL_POINTS, 0, static_cast<GLsizei>(entry.emitter->getMaxCount()));
  }

  glDisableVertexAttribArray(att_pointSize);
  glDisableVertexAttribArray(att_color);
  glDisableVertexAttribArray(att_coord);
}

}
}
}
