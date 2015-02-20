#include "Font.hpp"
#include <fstream>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Texture.hpp"
#include "Program.hpp"
#include "VBO.hpp"
#include "Game.hpp"
#include "GlUtils.hpp"

namespace Diggler {

const Program *Font::RenderProgram = nullptr;
GLint Font::RenderProgram_uni_mvp = -1;
GLint Font::RenderProgram_att_coord = -1;
GLint Font::RenderProgram_att_texcoord = -1;
GLint Font::RenderProgram_att_color = -1;
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
	if (!RenderProgram) {
		RenderProgram = G->PM->getProgram(PM_2D | PM_TEXTURED | PM_COLORED);
		RenderProgram_att_coord = RenderProgram->att("coord");
		RenderProgram_att_texcoord = RenderProgram->att("texcoord");
		RenderProgram_att_color = RenderProgram->att("color");
		RenderProgram_uni_mvp = RenderProgram->uni("mvp");
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

//#define eraseCurChar() std::fill_n((char*)&verts[i*6], sizeof(Vertex)*6, 0);
#define eraseCurChar() elements -= 6;

int Font::updateVBO(VBO &vbo, const std::string &text) const {
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
			/* Used to be '§', but wouldn't support UTF8
			Additionally, \f can mean "format" */
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
	vbo.setData(verts, elements, GL_STATIC_DRAW);
	delete[] verts;
	return elements;
}

void Font::draw(const Diggler::VBO &vbo, int count, const glm::mat4& matrix) const {
	glEnableVertexAttribArray(RenderProgram_att_coord);
	glEnableVertexAttribArray(RenderProgram_att_texcoord);
	glEnableVertexAttribArray(RenderProgram_att_color);
	
	RenderProgram->bind();
	m_texture->bind();
	vbo.bind();
	glUniformMatrix4fv(RenderProgram_uni_mvp, 1, GL_FALSE, glm::value_ptr(matrix));
	glVertexAttribPointer(RenderProgram_att_coord, 2, GL_INT, GL_FALSE, sizeof(Vertex), 0);
	glVertexAttribPointer(RenderProgram_att_texcoord, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, tx));
	glVertexAttribPointer(RenderProgram_att_color, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, r));
	glDrawArrays(GL_TRIANGLES, 0, count);
	
	glDisableVertexAttribArray(RenderProgram_att_color);
	glDisableVertexAttribArray(RenderProgram_att_texcoord);
	glDisableVertexAttribArray(RenderProgram_att_coord);
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

Font::~Font() {
	delete m_texture;
	if (texPos)
		delete[] texPos;
}

}