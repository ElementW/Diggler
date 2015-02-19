#include "EscMenu.hpp"
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Game.hpp"
#include "ui/Text.hpp"
#include "ui/Button.hpp"
#include "GameWindow.hpp"

namespace Diggler {

const Program *EscMenu::RenderProgram = nullptr;
GLint EscMenu::RenderProgram_att_color = -1;
GLint EscMenu::RenderProgram_att_coord = -1;
GLint EscMenu::RenderProgram_uni_mvp = -1;

static void makeBgVBO(VBO &vbo, int w, int h) {
	float verts[6*6] = {
		0.f, 0.f, 0.f, 0.f, 0.f, 0.6f,
		w  , 0.f, 0.f, 0.f, 0.f, 0.6f,
		0.f, h  , 0.f, 0.f, 0.f, 0.6f,
		
		0.f, h  , 0.f, 0.f, 0.f, 0.6f,
		w  , 0.f, 0.f, 0.f, 0.f, 0.6f,
		w  , h  , 0.f, 0.f, 0.f, 0.6f,
	};
	vbo.setData(verts, 6*6);
}

EscMenu::EscMenu(Game *G) : G(G) {
	makeBgVBO(vbo_background, G->GW->getW(), G->GW->getH());
	if (RenderProgram == nullptr) {
		RenderProgram = G->PM->getProgram(PM_2D | PM_COLORED);
		RenderProgram_att_coord = RenderProgram->att("coord");
		RenderProgram_att_color = RenderProgram->att("color");
		RenderProgram_uni_mvp = RenderProgram->uni("mvp");
	}
	
	//txt_quit = new Text();
	//m_button = new UIButton(G, glm::mat);
}

EscMenu::~EscMenu() {

}

void EscMenu::render() {
	glEnableVertexAttribArray(RenderProgram_att_coord);
	glEnableVertexAttribArray(RenderProgram_att_color);
	
	RenderProgram->bind();
	vbo_background.bind();
	//glm::mat4 trz = glm::translate(transform, glm::vec3(x/640.f, y/480.f, 0));
	glUniformMatrix4fv(RenderProgram_uni_mvp, 1, GL_FALSE, glm::value_ptr(*G->GW->UIM.PM));
	glVertexAttribPointer(RenderProgram_att_coord, 2, GL_FLOAT, GL_FALSE, 6*sizeof(float), 0);
	glVertexAttribPointer(RenderProgram_att_color, 4, GL_FLOAT, GL_FALSE, 6*sizeof(float), (GLvoid*)(2*sizeof(float)));
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
	glDisableVertexAttribArray(RenderProgram_att_color);
	glDisableVertexAttribArray(RenderProgram_att_coord);
	
	//txt_quit->render();
}

}