#ifndef CLOUDS_HPP
#define CLOUDS_HPP
#include <glm/glm.hpp>
#include <GL/glew.h>
#include "VBO.hpp"

namespace Diggler {

class Game;
class Program;
class Texture;

class Clouds {
private:
	static const Program *RenderProgram;
	static GLint RenderProgram_attrib_coord, RenderProgram_attrib_texcoord, RenderProgram_uni_mvp, RenderProgram_uni_texshift;
	Texture **m_tex;
	VBO m_vbo;
	int m_layers;
	Game *G;
	struct Coord { uint8 x, y, z, u, v; };

public:
	Clouds(Game *G, int w, int h, int layers);
	void render(const glm::mat4 &transform);
	~Clouds();
};

}

#endif