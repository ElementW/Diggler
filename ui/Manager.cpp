#include "Manager.hpp"
#include "../VBO.hpp"
#include "../Texture.hpp"
#include "../Program.hpp"
#include "../Game.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Diggler {
namespace UI {

static const Program *RP_Rect = nullptr;
static GLint RP_Rect_att_texcoord = -1;
static GLint RP_Rect_att_coord = -1;
static GLint RP_Rect_uni_mvp = -1;
static GLint RP_Rect_uni_unicolor = -1;

Manager::Manager() : Scale(2) {
	PM = &m_projMatrix;
	m_projMat1 = glm::ortho(0.f, 1.f, 0.f, 1.f);
	m_projMat1V = glm::ortho(0.f, 1.f, 1.f, 0.f);
	PM1 = &m_projMat1;
}

void Manager::setup(Game *G) {
	this->G = G;
	if (RP_Rect == nullptr) {
		RP_Rect = G->PM->getProgram(PM_2D | PM_TEXTURED);
		RP_Rect_att_coord = RP_Rect->att("coord");
		RP_Rect_att_texcoord = RP_Rect->att("texcoord");
		RP_Rect_uni_mvp = RP_Rect->uni("mvp");
		RP_Rect_uni_unicolor = RP_Rect->uni("unicolor");
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
	RP_Rect->bind();
	glEnableVertexAttribArray(RP_Rect_att_coord);

	Texture::unbind();
	m_rectVbo->bind();
	glUniform4f(RP_Rect_uni_unicolor, color.r, color.g, color.b, color.a);
	glUniformMatrix4fv(RP_Rect_uni_mvp, 1, GL_FALSE, glm::value_ptr(mat));
	glVertexAttribPointer(RP_Rect_att_coord, 2, GL_UNSIGNED_BYTE, GL_FALSE, 4*sizeof(uint8), 0);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	// OpenGL needs to be stateless. Definitely. Wait for Vulkan.
	glUniform4f(RP_Rect_uni_unicolor, 1.f, 1.f, 1.f, 1.f);

	glDisableVertexAttribArray(RP_Rect_att_coord);
}


void Manager::drawTex(const Element::Area &a, const Texture &t) const {
	drawTex(glm::scale(glm::translate(*PM, glm::vec3(a.x, a.y, 0)), glm::vec3(a.w, a.h, 0)), t);
}

void Manager::drawTex(const glm::mat4 &mat, const Texture &t) const {
	RP_Rect->bind();
	glEnableVertexAttribArray(RP_Rect_att_coord);
	glEnableVertexAttribArray(RP_Rect_att_texcoord);

	t.bind();
	m_rectVbo->bind();
	glUniformMatrix4fv(RP_Rect_uni_mvp, 1, GL_FALSE, glm::value_ptr(mat));
	glVertexAttribPointer(RP_Rect_att_coord, 2, GL_UNSIGNED_BYTE, GL_FALSE, 4*sizeof(uint8), 0);
	glVertexAttribPointer(RP_Rect_att_texcoord, 2, GL_UNSIGNED_BYTE, GL_FALSE, 4*sizeof(uint8), (void*)(2*sizeof(uint8)));
	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(RP_Rect_att_texcoord);
	glDisableVertexAttribArray(RP_Rect_att_coord);
}

void Manager::drawFullTexV(const Texture &t) const {
	drawTex(m_projMat1V, t);
}

}
}