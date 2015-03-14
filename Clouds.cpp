#include "Clouds.hpp"
#include "Texture.hpp"
#include "Platform.hpp"
#include "Program.hpp"
#include "Game.hpp"
#include <glm/gtc/type_ptr.hpp>

namespace Diggler {

const Program *Clouds::RenderProgram = nullptr;
GLint Clouds::RenderProgram_attrib_texcoord = -1;
GLint Clouds::RenderProgram_attrib_coord = -1;
GLint Clouds::RenderProgram_uni_mvp = -1;
GLint Clouds::RenderProgram_uni_texshift = -1;

Clouds::Clouds(Game *G, int w, int h, int layers) : m_layers(layers), G(G) {
	if (RenderProgram == nullptr) {
		RenderProgram = G->PM->getProgram(PM_3D | PM_FOG | PM_TEXTURED | PM_TEXSHIFT | PM_DISCARD);
		RenderProgram_attrib_coord = RenderProgram->att("coord");
		RenderProgram_attrib_texcoord = RenderProgram->att("texcoord");
		RenderProgram_uni_texshift = RenderProgram->uni("texshift");
		RenderProgram_uni_mvp = RenderProgram->uni("mvp");
	}

	m_tex = new Texture*[m_layers];
	uint8 *data = new uint8[w * h * 4];
	for (int i=0; i < m_layers; i++) {
		for (int x=0; x < w; x++) {
			for (int y=0; y < h; y++) {
				data[(x+y*w)*4+0] = data[(x+y*w)*4+1] = data[(x+y*w)*4+2] = 255;
				data[(x+y*w)*4+3] = (FastRand(0, 255) > 230) ? 180 : 0;
			}
		}
		m_tex[i] = new Texture(w, h, data, Texture::PixelFormat::RGBA);
	}
	delete[] data;

	Coord coords[m_layers * 6];
	for (int i=0; i < m_layers; i++) {
		coords[i*6+2] = {0, i, 0, 0, 0};
		
		coords[i*6+1] = coords[i*6+3] = {0, i, 1, 0, 1};
		coords[i*6+0] = coords[i*6+4] = {1, i, 0, 1, 0};
		
		coords[i*6+5] = {1, i, 1, 1, 1};
	}
	m_vbo.setData(coords, m_layers * 6);
}

void Clouds::render(const glm::mat4 &transform) {
	RenderProgram->bind();

	glEnableVertexAttribArray(RenderProgram_attrib_coord);
	glEnableVertexAttribArray(RenderProgram_attrib_texcoord);

	m_vbo.bind();
	glVertexAttribPointer(RenderProgram_attrib_coord, 3, GL_BYTE, GL_FALSE, sizeof(Coord), 0);
	glVertexAttribPointer(RenderProgram_attrib_texcoord, 2, GL_BYTE, GL_FALSE, sizeof(Coord), (GLvoid*)offsetof(Coord, u));

	float shift = G->Time/100;
	for (int i=m_layers-1; i >= 0; i--) {
		glUniformMatrix4fv(RenderProgram_uni_mvp, 1, GL_FALSE, glm::value_ptr(transform));
		glUniform2f(RenderProgram_uni_texshift, shift*(1+i*.5f), shift*(1+i*.5f));
		m_tex[i]->bind();
		glDrawArrays(GL_TRIANGLES, i*6, 6);
	}

	glDisableVertexAttribArray(RenderProgram_attrib_texcoord);
	glDisableVertexAttribArray(RenderProgram_attrib_coord);
}

Clouds::~Clouds() {
	for (int i=0; i < m_layers; i++) {
		delete m_tex[i];
	}
	delete[] m_tex;
}

}