#include "EscMenu.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Game.hpp"
#include "ui/Text.hpp"
#include "ui/Button.hpp"
#include "GameWindow.hpp"

namespace Diggler {

const Program *EscMenu::RenderProgram = nullptr;
GLint EscMenu::RenderProgram_uni_unicolor = -1;
GLint EscMenu::RenderProgram_att_coord = -1;
GLint EscMenu::RenderProgram_uni_mvp = -1;

static void makeBgVBO(VBO &vbo) {
	float verts[6*2] = {
		0.f, 0.f,
		1.f, 0.f,
		0.f, 1.f,
		
		0.f, 1.f,
		1.f, 0.f,
		1.f, 1.f,
	};
	vbo.setData(verts, 6*2);
}

EscMenu::EscMenu(Game *G) : G(G) {
	makeBgVBO(vbo_background);
	if (RenderProgram == nullptr) {
		RenderProgram = G->PM->getProgram(PM_2D);
		RenderProgram_att_coord = RenderProgram->att("coord");
		RenderProgram_uni_unicolor = RenderProgram->uni("unicolor");
		RenderProgram_uni_mvp = RenderProgram->uni("mvp");
	}
	
	txt_quit = G->UIM->add<UI::Text>(G->F, " Menu", 3, 3);
	txt_quit->setVisible(false);
	//m_button = new UIButton(G, glm::mat);
}

EscMenu::~EscMenu() {

}

void EscMenu::setVisible(bool v) {
	if (v) {
		m_transition.start = G->Time;
		m_transition.duration = 0.3;
		m_transition.active = true;
	}
}

static double easeOutQuart(double t, double d) {
	t /= d;
	t--;
	return -(t*t*t*t - 1);
}

void EscMenu::render() {
	double scroll;
	if (m_transition.active) {
		scroll = 1 - easeOutQuart(G->Time-m_transition.start, m_transition.duration)*.5;
		if (G->Time-m_transition.start >= m_transition.duration) {
			m_transition.active = false;
			txt_quit->setMatrix(nullptr);
		}
	} else {
		scroll = 0.5;
	}
	
	glEnableVertexAttribArray(RenderProgram_att_coord);
	
	RenderProgram->bind();
	vbo_background.bind();
	
	glm::mat4 trz = glm::translate(*G->GW->UIM.PM1, glm::vec3(scroll, 0, 0));
	glUniformMatrix4fv(RenderProgram_uni_mvp, 1, GL_FALSE, glm::value_ptr(trz));
	glUniform4f(RenderProgram_uni_unicolor, 0.f, 0.f, 0.f, 0.8f);
	glVertexAttribPointer(RenderProgram_att_coord, 2, GL_FLOAT, GL_FALSE, 0, 0);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
	glDisableVertexAttribArray(RenderProgram_att_coord);
	
	matrix = glm::translate(*G->GW->UIM.PM, glm::vec3(scroll*G->GW->getW(), G->GW->getH()-txt_quit->getSize().y, 0));
	txt_quit->setMatrix(&matrix);
	txt_quit->render();
}

}