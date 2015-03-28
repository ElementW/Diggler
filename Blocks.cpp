#include "Blocks.hpp"
#include "GlobalProperties.hpp"
#include "stb_perlin.h"

#define NON 0x0
#define RED Blocks::TeamRed
#define BLU Blocks::TeamBlue
#define ANY (RED | BLU)

namespace Diggler {

typedef BlockType Type;
const Blocks::TypeInfo Blocks::TypeInfos[(int)Type::LAST] = {
	{Type::Air, "Air",					0,		0,	0,		NON, "deconstruction.png"},
	{Type::Dirt, "Dirt",				0,		0,	0,		NON, nullptr},
	{Type::Ore, "Ore",					0,		25,	0,		NON, nullptr},
	{Type::Gold, "Gold",				100,	0,	0,		NON, nullptr},
	{Type::Diamond, "Diamond",			1000,	25,	0,		NON, nullptr},
	{Type::Rock, "Rock",				0,		0,	0,		NON, nullptr},
	{Type::Ladder, "Ladder",			0,		0,	25,		ANY, "ladder.png"},
	{Type::Explosive, "Explosives",		0,		0,	100,	ANY, "explosive.png"},
	{Type::Jump, "Jump pad",			0,		0,	25,		ANY, "jump.png"},
	{Type::Shock, "Shock Block",		0,		0,	50,		ANY, "spikes.png"},
	{Type::BankRed, "Bank",				0,		0,	50,		RED, "bank_red.png"},
	{Type::BankBlue, "Bank",			0,		0,	50,		BLU, "bank_blue.png"},
	{Type::BeaconRed, "Beacon",			0,		0,	50,		RED, "beacon.png"},
	{Type::BeaconBlue, "Beacon",		0,		0,	50,		BLU, "beacon.png"},
	{Type::Road, "Road",				0,		0,	10,		ANY, "road.png"},
	{Type::SolidRed, "Solid Block",		0,		0,	10,		RED, "solid_red.png"},
	{Type::SolidBlue, "Solid Block",	0,		0,	10,		BLU, "solid_blue.png"},
	{Type::Metal, "Metal Block",		0,		0,	0,		NON, "metal.png"},
	{Type::DirtSign, "Dirt",			0,		0,	0,		NON, nullptr},
	{Type::Lava, "Lava",				0,		0,	0,		NON, nullptr},
	{Type::TransRed, "Force Field",		0,		0,	25,		ANY, "translucent_red.png"},
	{Type::TransBlue, "Force Field",	0,		0,	25,		ANY, "translucent_blue.png"}
};

typedef BlockTex Tex;
const Tex sideTextures[(int)Type::LAST][6] = {
	/* Air */	{Tex::None, Tex::None, Tex::None, Tex::None, Tex::None, Tex::None},
	/* Dirt */	{Tex::Dirt, Tex::Dirt, Tex::Dirt, Tex::Dirt, Tex::Dirt, Tex::Dirt},
	/* Ore */	{Tex::Ore, Tex::Ore, Tex::Ore, Tex::Ore, Tex::Ore, Tex::Ore},
	/* Gold */	{Tex::Gold, Tex::Gold, Tex::Gold, Tex::Gold, Tex::Gold, Tex::Gold},
	/*Diamond*/	{Tex::Diamond, Tex::Diamond, Tex::Diamond, Tex::Diamond, Tex::Diamond, Tex::Diamond},
	/* Rock */	{Tex::Rock, Tex::Rock, Tex::Rock, Tex::Rock, Tex::Rock, Tex::Rock},
	/* Ladder */{Tex::Ladder, Tex::Ladder, Tex::LadderTop, Tex::LadderTop, Tex::Ladder, Tex::Ladder},
	/* TNT*/	{Tex::Explosive, Tex::Explosive, Tex::Explosive, Tex::Explosive, Tex::Explosive, Tex::Explosive},
	/* Jump */	{Tex::Jump, Tex::Jump, Tex::JumpTop, Tex::TeleBottom, Tex::Jump, Tex::Jump},
	/* Shock */	{Tex::TeleSideA, Tex::TeleSideA, Tex::TeleBottom, Tex::Spikes, Tex::TeleSideB, Tex::TeleSideB},
	/*BankRed*/	{Tex::BankFrontRed, Tex::BankBackRed, Tex::BankTopRed, Tex::BankTopRed, Tex::BankLeftRed, Tex::BankRightRed},
	/*BankBlue*/{Tex::BankFrontBlue, Tex::BankBackBlue, Tex::BankTopBlue, Tex::BankTopBlue, Tex::BankLeftBlue, Tex::BankRightBlue},
	/*BeaconR*/	{Tex::TeleSideA, Tex::TeleSideA, Tex::BeaconRed, Tex::LadderTop, Tex::TeleSideB, Tex::TeleSideB},
	/*BeaconB*/	{Tex::TeleSideA, Tex::TeleSideA, Tex::BeaconBlue, Tex::LadderTop, Tex::TeleSideB, Tex::TeleSideB},
	/* Road */	{Tex::Road, Tex::Road, Tex::Road, Tex::Road, Tex::Road, Tex::Road},
	/* SolidR */{Tex::SolidRed, Tex::SolidRed, Tex::SolidRed, Tex::SolidRed, Tex::SolidRed, Tex::SolidRed},
	/* SolidB */{Tex::SolidBlue, Tex::SolidBlue, Tex::SolidBlue, Tex::SolidBlue, Tex::SolidBlue, Tex::SolidBlue},
	/* Metal */	{Tex::Metal, Tex::Metal, Tex::Metal, Tex::Metal, Tex::Metal, Tex::Metal},
	/*DirtSign*/{Tex::DirtSign, Tex::DirtSign, Tex::DirtSign, Tex::DirtSign, Tex::DirtSign, Tex::DirtSign},
	/* Lava */	{Tex::Lava, Tex::Lava, Tex::Lava, Tex::Lava, Tex::Lava, Tex::Lava},
	/* TransR */{Tex::TransRed, Tex::TransRed, Tex::TransRed, Tex::TransRed, Tex::TransRed, Tex::TransRed},
	/* TransB */{Tex::TransBlue, Tex::TransBlue, Tex::TransBlue, Tex::TransBlue, Tex::TransBlue, Tex::TransBlue},
};

bool Blocks::isTransparent(BlockType t) {
	switch (t) {
	case BlockType::Air:
	case BlockType::TransBlue:
	case BlockType::TransRed:
		return true;
	default:
		return false;
	}
}

bool Blocks::isFaceVisible(BlockType t, BlockType other) {
	if (isTransparent(t)) {
		return (t != other);
	} else {
		return isTransparent(other);
	}
}

bool Blocks::canGoThrough(BlockType t, Player::Team team) {
	if (t == BlockType::Air)
		return true;
	return (t == BlockType::TransRed && team == Player::Team::Red) ||
		   (t == BlockType::TransBlue && team == Player::Team::Blue);
}

#define AddTex(b, t) m_coords[(int)b] = m_atlasCreator->add(getAssetPath("blocks", t)); 
Blocks::Blocks() : m_atlas(nullptr) {
	m_atlasCreator = new AtlasCreator(64*8, 64*8); //64*((int)Tex::LAST/8));
	m_coords = new AtlasCreator::Coord[Tex::LAST];

	AddTex(Tex::Dirt,			"tex_block_dirt.png");
	AddTex(Tex::DirtSign,		"tex_block_dirt_sign.png");
	AddTex(Tex::Rock,			"tex_block_rock.png");
	AddTex(Tex::Ore,			"tex_block_ore.png");
	AddTex(Tex::Gold,			"tex_block_silver.png");
	AddTex(Tex::Diamond,		"tex_block_diamond.png");
	AddTex(Tex::HomeRed,		"tex_block_home_red.png");
	AddTex(Tex::HomeBlue,		"tex_block_home_blue.png");
	AddTex(Tex::SolidRed,		"tex_block_red.png");
	AddTex(Tex::SolidBlue,		"tex_block_blue.png");
	AddTex(Tex::Ladder,		"tex_block_ladder.png");
	AddTex(Tex::LadderTop,		"tex_block_ladder_top.png");
	AddTex(Tex::Spikes,		"tex_block_spikes.png");
	AddTex(Tex::Jump,			"tex_block_jump.png");
	AddTex(Tex::JumpTop,		"tex_block_jump_top.png");
	AddTex(Tex::Explosive,		"tex_block_explosive.png");
	AddTex(Tex::Metal,			"tex_block_metal.png");
	AddTex(Tex::BankTopRed,	"tex_block_bank_top_red.png");
	AddTex(Tex::BankLeftRed,	"tex_block_bank_left_red.png");
	AddTex(Tex::BankFrontRed,	"tex_block_bank_front_red.png");
	AddTex(Tex::BankRightRed,	"tex_block_bank_right_red.png");
	AddTex(Tex::BankBackRed,	"tex_block_bank_back_red.png");
	AddTex(Tex::BankTopBlue,	"tex_block_bank_top_blue.png");
	AddTex(Tex::BankLeftBlue,	"tex_block_bank_left_blue.png");
	AddTex(Tex::BankFrontBlue,	"tex_block_bank_front_blue.png");
	AddTex(Tex::BankRightBlue,	"tex_block_bank_right_blue.png");
	AddTex(Tex::BankBackBlue,	"tex_block_bank_back_blue.png");
	AddTex(Tex::TeleSideA,		"tex_block_teleporter_a.png");
	AddTex(Tex::TeleSideB,		"tex_block_teleporter_b.png");
	AddTex(Tex::TeleTop,		"tex_block_teleporter_top.png");
	AddTex(Tex::TeleBottom,	"tex_block_teleporter_bottom.png");
	AddTex(Tex::Lava,			"tex_block_lava.png");
	AddTex(Tex::Road,			"tex_block_road_orig.png");
	AddTex(Tex::RoadTop,		"tex_block_road_top.png");
	AddTex(Tex::RoadBottom,	"tex_block_road_bottom.png");
	AddTex(Tex::BeaconRed,		"tex_block_beacon_top_red.png");
	AddTex(Tex::BeaconBlue,	"tex_block_beacon_top_blue.png");
	AddTex(Tex::TransRed,		"tex_block_trans_red.png");
	AddTex(Tex::TransBlue,		"tex_block_trans_blue.png");

	m_atlas = m_atlasCreator->getAtlas();
	delete m_atlasCreator;
}

Blocks::~Blocks() {
	if (m_atlas)
		delete m_atlas;
	delete[] m_coords;
}

const AtlasCreator::Coord* Blocks::gTC(BlockType t, FaceDirection d) const {
	int idx = (int)sideTextures[(int)t][(int)d];
	return &(m_coords[idx]);
}

Texture* Blocks::getAtlas() const {
	return m_atlas;
}

}