#ifndef FONT_HPP
#define FONT_HPP
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "Platform.hpp"
#include "VBO.hpp"

namespace Diggler {

class Texture;
class Program;
class Game;

class Font {
private:
	Texture *m_texture;
	struct CoordInfo {
		float x1, y1, x2, y2;
	} *m_coords;
	struct CharTexPos {
		int width;
		float left, right;
	} *texPos;
	uint8 height;
	static Program const *RenderProgram;
	static GLint RenderProgram_uni_mvp, RenderProgram_att_coord, RenderProgram_att_texcoord, RenderProgram_att_color;
	Game *G;
	
	Font(const Font &other) = delete;

public:
	struct Size { int x, y; };
	
	Font(Game *G, const std::string &path);
	
	/// Updates the text VBO
	/// @returns the number of elements in the VBO
	///
	int updateVBO(VBO &vbo, const std::string &text) const;
	void draw(const Diggler::VBO &vbo, int count, const glm::mat4& matrix) const;
	Size getSize(const std::string &text) const;
	~Font();
};

}

#endif