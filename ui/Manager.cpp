#include "Manager.hpp"
#include "../VBO.hpp"
#include "../Texture.hpp"
#include "../Program.hpp"
#include "../Game.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Diggler {
namespace UI {

struct Renderer {
	const Program *prog;
	GLint att_texcoord, att_coord, uni_mvp, uni_unicolor;
} R = {0}, RR = {0};

Manager::Manager() : Scale(2) {
	PM = &m_projMatrix;
	m_projMat1 = glm::ortho(0.f, 1.f, 0.f, 1.f);
	m_projMat1V = glm::ortho(0.f, 1.f, 1.f, 0.f);
	PM1 = &m_projMat1;
}

void Manager::setup(Game *G) {
	this->G = G;
	if (R.prog == nullptr) {
		R.prog = G->PM->getProgram(PM_2D | PM_TEXTURED);
		R.att_texcoord = R.prog->att("texcoord");
		R.att_coord = R.prog->att("coord");
		R.uni_mvp = R.prog->uni("mvp");
		R.uni_unicolor = R.prog->uni("unicolor");
	}
	if (RR.prog == nullptr) {
		RR.prog = G->PM->getProgram(PM_2D);
		RR.att_coord = RR.prog->att("coord");
		RR.uni_mvp = RR.prog->uni("mvp");
		RR.uni_unicolor = RR.prog->uni("unicolor");
	}
	m_rectVbo = new VBO();
	uint8 verts[6*4] = {
		0, 0, 0, 1,
		1, 0, 1, 1,
		0, 1, 0, 0,
		
		0, 1, 0, 0,
		1, 0, 1, 1,
		1, 1, 1, 0
	};
	m_rectVbo->setData(verts, 6*4);
}

void Manager::clear() {
	m_elements.clear();
}

void Manager::add(Element *e) {
	m_elements.emplace_back(e);
}


void Manager::remove(Element *e) {
	m_elements.remove_if([&e](_<Element> &l) -> bool { return l == e; });
}

void Manager::render() {
	for (_<Element>& e : m_elements) {
		if (e->m_isVisible)
			e->render();
	}
}

void Manager::setProjMat(const glm::mat4 &mat) {
	m_projMatrix = mat;
}

void Manager::drawRect(const glm::mat4 &mat, const glm::vec4 &color) const {
	RR.prog->bind();
	glEnableVertexAttribArray(RR.att_coord);

	m_rectVbo->bind();
	Texture::unbind();
	glUniform4f(RR.uni_unicolor, color.r, color.g, color.b, color.a);
	glUniformMatrix4fv(RR.uni_mvp, 1, GL_FALSE, glm::value_ptr(mat));
	glVertexAttribPointer(RR.att_coord, 2, GL_UNSIGNED_BYTE, GL_FALSE, 4*sizeof(uint8), 0);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// OpenGL needs to be stateless. Definitely. Wait for Vulkan.
	glUniform4f(RR.uni_unicolor, 1.f, 1.f, 1.f, 1.f);

	glDisableVertexAttribArray(RR.att_coord);
}

void Manager::drawFullRect(const glm::vec4 &color) const {
	drawRect(m_projMat1, color);
}

void Manager::drawTex(const glm::mat4 &mat, const Texture &t, const glm::vec4 &color) const {
	R.prog->bind();
	glEnableVertexAttribArray(R.att_coord);
	glEnableVertexAttribArray(R.att_texcoord);

	t.bind();
	m_rectVbo->bind();
	glUniform4f(R.uni_unicolor, color.r, color.g, color.b, color.a);
	glUniformMatrix4fv(R.uni_mvp, 1, GL_FALSE, glm::value_ptr(mat));
	glVertexAttribPointer(R.att_coord, 2, GL_UNSIGNED_BYTE, GL_FALSE, 4*sizeof(uint8), 0);
	glVertexAttribPointer(R.att_texcoord, 2, GL_UNSIGNED_BYTE, GL_FALSE, 4*sizeof(uint8), (void*)(2*sizeof(uint8)));
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glUniform4f(R.uni_unicolor, 1.f, 1.f, 1.f, 1.f);

	glDisableVertexAttribArray(R.att_texcoord);
	glDisableVertexAttribArray(R.att_coord);
}

void Manager::drawTex(const glm::mat4 &mat, const Texture &t) const {
	drawTex(mat, t, glm::vec4(1.f));
}

void Manager::drawTex(const Element::Area &a, const Texture &t) const {
	drawTex(glm::scale(glm::translate(*PM, glm::vec3(a.x, a.y, 0)), glm::vec3(a.w, a.h, 0)), t);
}

void Manager::drawFullTexV(const Texture &t) const {
	drawTex(m_projMat1V, t);
}

void Manager::drawFullTexV(const Texture &t, const glm::vec4 &color) const {
	drawTex(m_projMat1V, t, color);
}

}
}