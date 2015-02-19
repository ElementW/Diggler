#include "Texture.hpp"
#include "Player.hpp"
#include "Game.hpp"
#include "GlobalProperties.hpp"
#include <glm/gtc/matrix_transform.hpp> 
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>

namespace Diggler {

Texture ***Player::Textures = nullptr;
const Program *Player::RenderProgram = nullptr;
GLint Player::RenderProgram_attrib_texcoord = -1;
GLint Player::RenderProgram_attrib_coord = -1;
GLint Player::RenderProgram_uni_mvp = -1;

const char* Player::getTeamNameLowercase(Player::Team t) {
	switch (t) {
		case Team::Blue:
			return "blue";
		case Team::Red:
			return "red";
		default:
			return "";
	}
}

const char* Player::getToolNameLowercase(Player::Tools t) {
	switch (t) {
		case Tools::ConstructionGun:
		case Tools::DeconstructionGun:
			return "construction";
		case Tools::Detonator:
			return "detonator";
		case Tools::Pickaxe:
			return "pickaxe";
		case Tools::ProspectingRadar:
			return "radar";
		default:
			return "";
	}
}

int Player::getMaxOre(Class c) {
	switch (c) {
		case Class::Engineer:
			return 350;
		case Class::Miner:
		case Class::Prospector:
		case Class::Sapper:
			return 200;
	}
	return 0;
}

int Player::getMaxWeight(Class c) {
	switch (c) {
		case Class::Miner:
			return 8;
		case Class::Engineer:
		case Class::Prospector:
		case Class::Sapper:
			return 4;
	}
	return 0;
}

Player::Player(Game *G) : m_vbo(nullptr), team(Team::Red),
	playerclass(Class::Prospector), tool(Tools::Pickaxe), G(G) {
	if (GlobalProperties::IsClient) {
		m_vbo = new VBO();
		if (Textures == nullptr) {
			RenderProgram = G->PM->getProgram(PM_3D | PM_TEXTURED);
			RenderProgram_attrib_coord = RenderProgram->att("coord");
			RenderProgram_attrib_texcoord = RenderProgram->att("texcoord");
			RenderProgram_uni_mvp = RenderProgram->uni("mvp");
			
			Textures = new Texture**[Team::LAST-1];
			for (uint8 t=1; t < (uint8)Team::LAST-1; t++) {
				Textures[t] = new Texture*[Tools::LAST];
				for (uint8 tool=0; tool < (uint8)Tools::LAST; tool++) {
					std::string aa = std::string("tex_sprite_") + getTeamNameLowercase(team) + '_' + getToolNameLowercase(this->tool) + ".png";
					Textures[t][tool] = new Texture(
						getAssetPath("sprites", std::string("tex_sprite_") + getTeamNameLowercase((Team)t) + '_' + getToolNameLowercase((Tools)tool) + ".png"),
						Texture::PixelFormat::RGBA);
				}
			}
		}
		float coords[6*5] = {
			-.5f, 0.0f, 0.0f,  0.0f, 1.0f,
			0.5f, 0.0f, 0.0f,  1.0f, 1.0f,
			0.5f, 1.0f, 0.0f,  1.0f, 0.0f,
			
			-.5f, 1.0f, 0.0f,  0.0f, 0.0f,
			-.5f, 0.0f, 0.0f,  0.0f, 1.0f,
			0.5f, 1.0f, 0.0f,  1.0f, 0.0f,
		};
		m_vbo->setData(coords, 6*5);
	}
}

using std::swap;
Player::Player(Player &&p) : m_vbo(nullptr) {
	*this = std::move(p);
}

Player& Player::operator=(Player &&p) {
	swap(m_vbo, p.m_vbo);
	swap(team, p.team);
	swap(playerclass, p.playerclass);
	swap(tool, p.tool);
	swap(direction, p.direction);
	swap(G, p.G);
	swap(position, p.position);
	swap(velocity, p.velocity);
	swap(accel, p.accel);
	swap(name, p.name);
	swap(id, p.id);
	swap(P, p.P);
	return *this;
}

Player::~Player() {
	if (GlobalProperties::IsClient) {
		delete m_vbo; m_vbo = nullptr;
	}
}

void Player::setPosVel(const glm::vec3 &pos, const glm::vec3 &vel, const glm::vec3 &acc) {
	position = m_predictPos = pos;
	velocity = vel;
	accel = acc;
}

void Player::update(const float &delta) {
	velocity += accel * delta;
	m_predictPos += velocity * delta;
}

void Player::render(const glm::mat4 &transform) const {
	RenderProgram->bind();
	Textures[(uint8)team][(uint8)tool]->bind();
	m_vbo->bind();
	glEnableVertexAttribArray(RenderProgram_attrib_texcoord);
	glEnableVertexAttribArray(RenderProgram_attrib_coord);
	glm::vec3 &lpPos = G->LP->position;
	float angle = atan2(lpPos.x-m_predictPos.x, lpPos.z-m_predictPos.z);
	glUniformMatrix4fv(RenderProgram_uni_mvp, 1, GL_FALSE, glm::value_ptr(
		glm::translate(transform, m_predictPos) * glm::rotate(angle, glm::vec3(0.0, 1.0, 0.0))));
	glVertexAttribPointer(RenderProgram_attrib_coord, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), 0);
	glVertexAttribPointer(RenderProgram_attrib_texcoord, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (GLvoid*)(3*sizeof(float)));
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisableVertexAttribArray(RenderProgram_attrib_coord);
	glDisableVertexAttribArray(RenderProgram_attrib_texcoord);
}

}