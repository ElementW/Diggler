#include "Font.hpp"
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "../Texture.hpp"
#include "../Program.hpp"
#include "../VBO.hpp"
#include "../Game.hpp"
#include "../GlUtils.hpp"

namespace Diggler {
namespace UI {

Font::Renderer Font::R = {0};

static const struct { float r, g, b; } ColorTable[16] = {
	{1.0f, 1.0f, 1.0f},
	{0.66f, 0.66f, 0.66f},
	{0.33f, 0.33f, 0.33f},
	{0.0f, 0.0f, 0.0f},
	{1.0f, 0.0f, 0.0f},
	{1.0f, 0.5f, 0.0f},
	{1.0f, 1.0f, 0.0f},
	{0.5f, 1.0f, 0.0f},
	{0.0f, 1.0f, 0.0f},
	{0.0f, 1.0f, 0.5f},
	{0.0f, 1.0f, 1.0f},
	{0.0f, 0.5f, 1.0f},
	{0.0f, 0.0f, 1.0f},
	{0.5f, 0.0f, 1.0f},
	{1.0f, 0.0f, 1.0f},
	{1.0f, 0.0f, 0.5f}
};

Font::Font(Game *G, const std::string& path) : G(G) {
	if (!R.prog) {
		R.prog = G->PM->getProgram(PM_2D | PM_TEXTURED | PM_COLORED);
		R.att_coord = R.prog->att("coord");
		R.att_texcoord = R.prog->att("texcoord");
		R.att_color = R.prog->att("color");
		R.uni_mvp = R.prog->uni("mvp");
	}
	m_texture = new Texture(path, Texture::PixelFormat::RGBA);
	std::ifstream source(path + ".fdf", std::ios_base::binary);
	if (source.good()) {
		source.seekg(0, std::ios_base::end);
		int size = (int)source.tellg() - 1; // Last byte is font's height
		source.seekg(0, std::ios_base::beg);
		//widths = new uint8[size];
		texPos = new CharTexPos[95];
		uint8 widths[95];
		std::fill_n(widths, 95, 6);
		source.read((char*)widths, size);
		source.read((char*)&height, 1);
		int left = 0;
		for (uint8 i=0; i < 95; i++) {
			texPos[i].width = widths[i];
			texPos[i].left = (float)left / m_texture->getW();
			left += widths[i];
			texPos[i].right = (float)left / m_texture->getW();
		}
	}
}

struct Vertex { int x, y; float tx, ty; float r, g, b ,a; };

#define eraseCurChar() elements -= 6;

int Font::updateVBO(VBO &vbo, const std::string &text, GLenum usage) const {
	int elements = text.size()*6;
	Vertex *verts = new Vertex[elements];
	uint8 c, w; int line = 0, cx = 0, v = 0; float l, r;
	float cr = 1.0f, cg = 1.0f, cb = 1.0f, ca = 1.0f;
	for (uint i=0; i < text.size(); i++) {
		c = text[i];
		if (c == '\n') {
			eraseCurChar();
			line -= height;
			cx = 0;
			continue;
		}
		if (c == '\f' && i < text.length()) {
			/* \f can mean "format" */
			eraseCurChar();
			uint8 arg = text[++i];
			eraseCurChar();
			if (arg >= '0' && arg <= '9') {
				uint8 index = arg - '0';
				cr = ColorTable[index].r; cg = ColorTable[index].g; cb = ColorTable[index].b;
			} else if (arg >= 'a' && arg <= 'f') {
				uint8 index = (arg - 'a') + 10;
				cr = ColorTable[index].r; cg = ColorTable[index].g; cb = ColorTable[index].b;
			} else if (arg == 't') { // 't'ransparency
				uint8 arg2 = text[++i];
				eraseCurChar();
				if (arg2 >= 'a' && arg2 <= 'z')
					ca = (float)(arg2 - 'a')/('z'-'a');
			}
			continue;
		}
		if (c < ' ' || c > '~') {
			eraseCurChar();
			continue;
		}
		w = texPos[c - ' '].width;
		l = texPos[c - ' '].left;
		r = texPos[c - ' '].right;
		verts[v+0] = { cx, line, l, 1.0, cr, cg, cb, ca };
		verts[v+1] = { cx+w, line+height, r, 0.0, cr, cg, cb, ca };
		verts[v+2] = { cx, line+height, l, 0.0, cr, cg, cb, ca };
		verts[v+3] = { cx, line, l, 1.0, cr, cg, cb, ca };
		verts[v+4] = { cx+w, line, r, 1.0, cr, cg, cb, ca };
		verts[v+5] = { cx+w, line+height, r, 0.0, cr, cg, cb, ca };
		v += 6;
		cx += w;
	}
	vbo.setData(verts, elements, usage);
	delete[] verts;
	return elements;
}

void Font::draw(const VBO &vbo, int count, const glm::mat4& matrix) const {
	glEnableVertexAttribArray(R.att_coord);
	glEnableVertexAttribArray(R.att_texcoord);
	glEnableVertexAttribArray(R.att_color);

	R.prog->bind();
	m_texture->bind();
	vbo.bind();
	glUniformMatrix4fv(R.uni_mvp, 1, GL_FALSE, glm::value_ptr(matrix));
	glVertexAttribPointer(R.att_coord, 2, GL_INT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(R.att_texcoord, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, tx));
	glVertexAttribPointer(R.att_color, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, r));
	glDrawArrays(GL_TRIANGLES, 0, count);

	glDisableVertexAttribArray(R.att_color);
	glDisableVertexAttribArray(R.att_texcoord);
	glDisableVertexAttribArray(R.att_coord);
}

Font::Size Font::getSize(const std::string &text) const {
	uint8 c; int x = 0, y = height, cx = 0;
	for (uint i=0; i < text.size(); i++) {
		c = text[i];
		if (c == '\n') {
			y += height;
			if (cx > x)
				x = cx;
			cx = 0;
			continue;
		}
		if (c == '\f' && i < text.length()) { /* 167 = '§' */
			uint8 arg = text[++i];
			if (arg == 't') {
				i++;
			}
			continue;
		}
		x += texPos[c - ' '].width;
	}
	return Size { x, y };
}

int Font::getHeight() const {
	return height;
}

Font::~Font() {
	delete m_texture;
	if (texPos)
		delete[] texPos;
}

}
}