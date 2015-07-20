#ifndef PLAYER_HPP
#define PLAYER_HPP
#include "Platform.hpp"
#include <list>
#include <glm/glm.hpp>
#include <epoxy/gl.h>
#include "network/Network.hpp"
#include "World.hpp"

namespace Diggler {

class Program;
class VBO;
class Game;
class Texture;

class Player {
protected:
	static struct TexInfo {
		Texture *tex;
		union VBOs {
			struct NamedVBOs {
				VBO *walk1, *idle, *walk2, *fire;
			};
			VBO *vbos[4];
		} side[4];
	} ***TexInfos;
	static struct Renderer {
		const Program *prog;
		GLint att_coord,
			  att_texcoord,
			  uni_mvp,
			  uni_unicolor,
			  uni_fogStart,
			  uni_fogEnd;
	} R;
	double m_lastPosTime;
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
	WorldRef W;
	glm::vec3 position, lastPosition, velocity, accel;
	float angle; double toolUseTime;
	std::string name;
	uint32 id;
	bool isAlive;
	Net::Peer P;
	std::list<ChunkRef> pendingChunks;

	int ore, loot;

	static const char* getTeamNameLowercase(Team t);
	static const char* getToolNameLowercase(Tools t);
	static int getMaxOre(Class c);
	static int getMaxWeight(Class c);

	Player(Game *G = nullptr);
	Player(Player&&);
	Player& operator=(Player&&);
	~Player();

	void setPosVel(const glm::vec3 &pos, const glm::vec3 &vel, const glm::vec3 &acc = glm::vec3());
	void update(const float &delta);
	void render(const glm::mat4 &transform) const;
	void setDead(bool, DeathReason = DeathReason::None, bool send = false);
};

}

#endif