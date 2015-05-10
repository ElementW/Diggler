#include "Particles.hpp"
#include <glm/gtc/type_ptr.hpp>
#include "Platform.hpp"
#include "Game.hpp"
#include "GlUtils.hpp"

namespace Diggler {

ParticleEmitter::Renderer ParticleEmitter::R = {0};
struct GLParticle {
	float x, y, z, r, g, b, a, s;
};

ParticleEmitter::ParticleEmitter(Game *G) : G(G) {
	init();
}

void ParticleEmitter::init() {
	if (R.prog == nullptr) {
		R.prog = G->PM->getProgram(PM_3D | PM_POINTSIZE | PM_COLORED | PM_FOG);
		R.att_coord = R.prog->att("coord");
		R.att_color = R.prog->att("color");
		//R.att_texcoord = R.prog->att("texcoord");
		R.att_pointSize = R.prog->att("pointSize");
		R.uni_mvp = R.prog->uni("mvp");
		R.uni_unicolor = R.prog->uni("unicolor");
		R.uni_fogStart = R.prog->uni("fogStart");
		R.uni_fogEnd = R.prog->uni("fogEnd");
	}
	maxCount = 0;
}

void ParticleEmitter::setMaxCount(uint count) {
	vbo.setSize(sizeof(GLParticle)*count, GL_DYNAMIC_DRAW);
	particles.reserve(count);
	for (uint i=maxCount; i < count; ++i) {
		particles[i].decay = -1;
	}
	maxCount = count;
}

uint ParticleEmitter::getMaxCount() const {
	return maxCount;
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
	GLParticle *data = new GLParticle[maxCount];
	float deltaF = delta;
	for (int i=0; i < maxCount; ++i) {
		Particle &p = particles[i];
		p.vel += p.accel * deltaF;
		p.pos += p.vel * deltaF;
		p.decay -= deltaF;
		if (p.decay < 0)
			emit(p);
		data[i] = { p.pos.x, p.pos.y, p.pos.z, p.color.r, p.color.g, p.color.b, p.color.a, p.size };
	}
	vbo.setSubData(data, 0, maxCount);
	delete[] data;
}

void ParticleEmitter::render(const glm::mat4 &transform) {
	R.prog->bind();
	vbo.bind();
	glEnableVertexAttribArray(R.att_coord);
	glEnableVertexAttribArray(R.att_color);
	glEnableVertexAttribArray(R.att_pointSize);
	glUniformMatrix4fv(R.uni_mvp, 1, GL_FALSE, glm::value_ptr(transform));
	glUniform1f(R.uni_fogStart, G->RP->fogStart);
	glUniform1f(R.uni_fogEnd, G->RP->fogEnd);
	glUniform4f(R.uni_unicolor, 1.f, 1.f, 1.f, 1.f);

	glVertexAttribPointer(R.att_coord, 3, GL_FLOAT, GL_FALSE, sizeof(GLParticle), 0);
	glVertexAttribPointer(R.att_color, 4, GL_FLOAT, GL_FALSE, sizeof(GLParticle), (GLvoid*)(3*sizeof(float)));
	glVertexAttribPointer(R.att_pointSize, 1, GL_FLOAT, GL_FALSE, sizeof(GLParticle), (GLvoid*)(7*sizeof(float)));
	glDrawArrays(GL_POINTS, 0, maxCount);

	glDisableVertexAttribArray(R.att_pointSize);
	glDisableVertexAttribArray(R.att_color);
	glDisableVertexAttribArray(R.att_coord);
}

}