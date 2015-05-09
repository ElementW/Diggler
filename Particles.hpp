#ifndef PARTICLES_HPP
#define PARTICLES_HPP
#include <vector>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <glm/mat4x4.hpp>
#include "Program.hpp"
#include "VBO.hpp"

namespace Diggler {

class Game;

struct Particle {
	// Keep it a POD struct
	glm::vec3 pos, vel, accel;
	glm::vec4 color;
	float size, decay;
};

class ParticleEmitter {
	static struct Renderer {
		const Program *prog;
		GLint att_coord,
			  att_color,
			  att_texcoord,
			  att_pointSize,
			  uni_mvp,
			  uni_unicolor,
			  uni_fogStart,
			  uni_fogEnd;
	} R;
	Game *G;
	void init();
	
	int count;
	std::vector<Particle> particles;
	VBO vbo;
public:
	Particle pTemplate;
	glm::vec3 pos, posAmpl;
	int maxCount;

	ParticleEmitter(Game*);

	void emit(Particle&);
	void update(double delta);
	void render(const glm::mat4&);
};

}

#endif