#ifndef PLAYER_HPP
#define PLAYER_HPP
#include "Platform.hpp"
#include <glm/glm.hpp>
#include "network/Network.hpp"
#include <GL/glew.h>

namespace Diggler {

class Program;
class VBO;
class Game;
class Texture;

class Player {
private:
	static struct TexInfo {
		Texture *tex;
		union VBOs {
			struct {
				VBO *walk1, *idle, *walk2, *fire;
			};
			VBO *vbos[4];
		} side[4];
	} ***TexInfos;
	static const Program *RenderProgram;
	static GLint RenderProgram_attrib_coord, RenderProgram_attrib_texcoord, RenderProgram_uni_mvp;
	glm::vec3 m_predictPos;
	
	Player(const Player&) = delete;
	Player& operator=(const Player&) = delete;

public:
	enum Team : uint8 {
		Red,
		Blue,
		LAST
	} team;
	enum class Class : uint8 {
		Prospector,
		Miner,
		Engineer,
		Sapper
	} playerclass;
	enum class Tools : uint8 {
		Pickaxe,
		ConstructionGun,
		DeconstructionGun,
		ProspectingRadar,
		Detonator,
		LAST
	} tool;
	enum class Direction : uint8 {
		North,	// To +Z
		East,	// To +X
		South,	// To -X
		West	// To -Z
	} direction;
	enum class DeathReason : uint8 {
		None,
		Lava,
		Shock,
		Fall,
		Explosion,
		Void
	} deathReason;
	Game *G;
	glm::vec3 position, velocity, accel;
	std::string name;
	uint32 id;
	bool isAlive;
	Net::Peer P;

	int ore, loot;

	static const char* getTeamNameLowercase(Team t);
	static const char* getToolNameLowercase(Tools t);
	static int getMaxOre(Class c);
	static int getMaxWeight(Class c);

	Player(Game *G = nullptr);
	Player(Player&&);
	Player& operator=(Player&&);
	virtual ~Player();

	virtual void setPosVel(const glm::vec3 &pos, const glm::vec3 &vel, const glm::vec3 &acc = glm::vec3());
	virtual void update(const float &delta);
	virtual void render(const glm::mat4 &transform) const;
	virtual void setDead(bool, DeathReason = DeathReason::None, bool send = false);
};

}

#endif