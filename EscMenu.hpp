#ifndef ESC_MENU_HPP
#define ESC_MENU_HPP
#include <GL/glew.h>
#include <glm/glm.hpp>
#include "VBO.hpp"
#include "Program.hpp"

namespace Diggler {

class Game;
namespace UI {
class Button;
class Text;
}

class EscMenu {
private:
	static const Program *RenderProgram;
	static GLint RenderProgram_att_coord, RenderProgram_uni_unicolor, RenderProgram_uni_mvp;
	Game *G;
	struct {
		double start, duration;
		bool active;
	} m_transition;
	VBO vbo_background;
	UI::Text *txt_quit;
	glm::mat4 matrix;
	UI::Button *m_button;

public:
	EscMenu(Game *G);
	~EscMenu();
	
	void setVisible(bool);
	void render();
};

}

#endif