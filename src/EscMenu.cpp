#include "EscMenu.hpp"
#include <epoxy/gl.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "Game.hpp"
#include "ui/Text.hpp"
#include "ui/Button.hpp"
#include "GameWindow.hpp"

namespace Diggler {

EscMenu::EscMenu(Game *G) : G(G) {
	txt_quit = G->UIM->add<UI::Text>(" Menu", 3, 3);
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
		scroll = easeOutQuart(G->Time-m_transition.start, m_transition.duration)*.5;
		if (G->Time-m_transition.start >= m_transition.duration) {
			m_transition.active = false;
			txt_quit->setMatrix(nullptr);
		}
	} else {
		scroll = 0.5;
	}

	int width = G->UIM->scale*256;
	int pxScroll = G->GW->getW()-scroll*width;
	matrix = glm::scale(glm::translate(*G->GW->UIM.PM, glm::vec3(pxScroll, 0, 0)), glm::vec3(width, G->GW->getH(), 0));
	G->UIM->drawRect(matrix, glm::vec4(0.f, 0.f, 0.f, 0.8f));
	
	matrix = glm::translate(*G->GW->UIM.PM, glm::vec3(pxScroll, G->GW->getH()-txt_quit->getSize().y, 0));
	txt_quit->setMatrix(&matrix);
	txt_quit->render();
}

}