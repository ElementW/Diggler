#ifndef BLOCKS_HPP
#define BLOCKS_HPP
#include <stdint.h>
#include "AtlasCreator.hpp"
#include "Player.hpp"

namespace Diggler {

enum class BlockType : uint8_t {
	Air = 0,
	Dirt = 1,
	Ore,
	Gold,
	Diamond,
	Rock,
	Ladder,
	Explosive,
	Jump,
	Shock,
	BankRed,
	BankBlue,
	BeaconRed,
	BeaconBlue,
	Road,
	SolidRed,
	SolidBlue,
	Metal,
	DirtSign,
	Lava,
	TransRed,
	TransBlue,
	LAST
};

enum class FaceDirection : uint8_t {
	XInc = 0,
	XDec = 1,
	YInc = 2,
	YDec = 3,
	ZInc = 4,
	ZDec = 5
};

enum class BlockTex : uint8_t {
	None = 0,
	Dirt,
	Ore,
	Gold,
	Diamond,
	Rock,
	Jump,
	JumpTop,
	Ladder,
	LadderTop,
	Explosive,
	Spikes,
	HomeRed,
	HomeBlue,
	BankTopRed,
	BankTopBlue,
	BankFrontRed,
	BankFrontBlue,
	BankLeftRed,
	BankLeftBlue,
	BankRightRed,
	BankRightBlue,
	BankBackRed,
	BankBackBlue,
	TeleTop,
	TeleBottom,
	TeleSideA,
	TeleSideB,
	SolidRed,
	SolidBlue,
	Metal,
	DirtSign,
	Lava,
	Road,
	RoadTop,
	RoadBottom,
	BeaconRed,
	BeaconBlue,
	TransRed,
	TransBlue,
	LAST
};

inline bool operator!(BlockType& t) {
	return !static_cast<uint8_t>(t);
}

class Blocks {
private:
	// Client
	AtlasCreator *m_atlasCreator;
	Texture *m_atlas;
	AtlasCreator::Coord *m_coords;

	// Shared
	;

	// No copy
	Blocks(const Blocks&) = delete;
	Blocks& operator=(const Blocks&) = delete;

public:
	constexpr static int TeamRed  = 0x1,
						 TeamBlue = 0x2;
	const static struct TypeInfo {
		BlockType type;
		const char *name;
		int cashWorth, oreWorth, buildCost;
		int teamCanBuild;
		const char *icon;
	} TypeInfos[(int)BlockType::LAST];

	Blocks();
	~Blocks();
	static bool isTransparent(BlockType t);
	static bool isFaceVisible(BlockType t, BlockType other);
	static bool canGoThrough(BlockType t, Player::Team team);
	const AtlasCreator::Coord* gTC(BlockType, FaceDirection) const;
	Texture* getAtlas() const;
};

}

#endif