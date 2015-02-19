#include "Blocks.hpp"
#include "GlobalProperties.hpp"

namespace Diggler {

BlockTex sideTextures[(int)BlockType::LAST][6] = {
	/* Air */	{BlockTex::None, BlockTex::None, BlockTex::None, BlockTex::None, BlockTex::None, BlockTex::None},
	/* Dirt */	{BlockTex::Dirt, BlockTex::Dirt, BlockTex::Dirt, BlockTex::Dirt, BlockTex::Dirt, BlockTex::Dirt},
	/* Ore */	{BlockTex::Ore, BlockTex::Ore, BlockTex::Ore, BlockTex::Ore, BlockTex::Ore, BlockTex::Ore},
	/* Gold */	{BlockTex::Gold, BlockTex::Gold, BlockTex::Gold, BlockTex::Gold, BlockTex::Gold, BlockTex::Gold},
	/*Diamond*/	{BlockTex::Diamond, BlockTex::Diamond, BlockTex::Diamond, BlockTex::Diamond, BlockTex::Diamond, BlockTex::Diamond},
	/* Rock */	{BlockTex::Rock, BlockTex::Rock, BlockTex::Rock, BlockTex::Rock, BlockTex::Rock, BlockTex::Rock},
	/* Ladder */{BlockTex::Ladder, BlockTex::Ladder, BlockTex::LadderTop, BlockTex::LadderTop, BlockTex::Ladder, BlockTex::Ladder},
	/* TNT*/	{BlockTex::Explosive, BlockTex::Explosive, BlockTex::Explosive, BlockTex::Explosive, BlockTex::Explosive, BlockTex::Explosive},
	/* Jump */	{BlockTex::Jump, BlockTex::Jump, BlockTex::JumpTop, BlockTex::TeleBottom, BlockTex::Jump, BlockTex::Jump},
	/* Shock */	{BlockTex::TeleSideA, BlockTex::TeleSideA, BlockTex::TeleBottom, BlockTex::Spikes, BlockTex::TeleSideB, BlockTex::TeleSideB},
	/*BankRed*/	{BlockTex::BankFrontRed, BlockTex::BankBackRed, BlockTex::BankTopRed, BlockTex::BankTopRed, BlockTex::BankLeftRed, BlockTex::BankRightRed},
	/*BankBlue*/{BlockTex::BankFrontBlue, BlockTex::BankBackBlue, BlockTex::BankTopBlue, BlockTex::BankTopBlue, BlockTex::BankLeftBlue, BlockTex::BankRightBlue},
	/*BeaconR*/	{BlockTex::TeleSideA, BlockTex::TeleSideA, BlockTex::BeaconRed, BlockTex::LadderTop, BlockTex::TeleSideB, BlockTex::TeleSideB},
	/*BeaconB*/	{BlockTex::TeleSideA, BlockTex::TeleSideA, BlockTex::BeaconBlue, BlockTex::LadderTop, BlockTex::TeleSideB, BlockTex::TeleSideB},
	/* Road */	{BlockTex::Road, BlockTex::Road, BlockTex::Road, BlockTex::Road, BlockTex::Road, BlockTex::Road},
	/* SolidR */{BlockTex::SolidRed, BlockTex::SolidRed, BlockTex::SolidRed, BlockTex::SolidRed, BlockTex::SolidRed, BlockTex::SolidRed},
	/* SolidB */{BlockTex::SolidBlue, BlockTex::SolidBlue, BlockTex::SolidBlue, BlockTex::SolidBlue, BlockTex::SolidBlue, BlockTex::SolidBlue},
	/* Metal */	{BlockTex::Metal, BlockTex::Metal, BlockTex::Metal, BlockTex::Metal, BlockTex::Metal, BlockTex::Metal},
	/*DirtSign*/{BlockTex::DirtSign, BlockTex::DirtSign, BlockTex::DirtSign, BlockTex::DirtSign, BlockTex::DirtSign, BlockTex::DirtSign},
	/* Lava */	{BlockTex::Lava, BlockTex::Lava, BlockTex::Lava, BlockTex::Lava, BlockTex::Lava, BlockTex::Lava},
	/* TransR */{BlockTex::TransRed, BlockTex::TransRed, BlockTex::TransRed, BlockTex::TransRed, BlockTex::TransRed, BlockTex::TransRed},
	/* TransB */{BlockTex::TransBlue, BlockTex::TransBlue, BlockTex::TransBlue, BlockTex::TransBlue, BlockTex::TransBlue, BlockTex::TransBlue},
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

bool Blocks::isFaceRemoved(BlockType t, BlockType other) {
	if (isTransparent(t)) {
		return (t != other);
	} else {
		return isTransparent(other);
	}
}

bool Blocks::canGoThrough(BlockType t, Player::Team team) {
	if (t == BlockType::Air)
		return true;
	if (t != BlockType::TransRed && t != BlockType::TransBlue)
		return false;
	return (t == BlockType::TransRed && team == Player::Team::Red) ||
		   (t == BlockType::TransBlue && team == Player::Team::Blue);
}

int32 blend(int32 c1, int32 c2, uint8 val) {
	uint ival = 256 - val;
	uint v1_1 = c1 & 0xFF00FF;
	uint v1_2 = c1 & 0x00FF00;
	uint v2_1 = c2 & 0xFF00FF;
	uint v2_2 = c2 & 0x00FF00;
	uint res = 
	( ( ( ( v1_1 * ival ) + ( v2_1 * val ) ) >> 8 ) & 0xFF00FF ) |
	( ( ( ( v1_2 * ival ) + ( v2_2 * val ) ) >> 8 ) & 0x00FF00 );
    return res;
}

int lerp(int a, int b, float x) {
	return a*(1-x) + b*x;
}
void makePerlin(int w, int h, uint8 *buf) {
	uint8 noise[(h/4)*(w/4)];
	for (int i=0; i < (h/4)*(w/4); i++)
		noise[i] = FastRand(255);
	for (int x=0; x < w; x++) {
		for (int y=0; y < h; y++) {
			int target = (x/4) + (y/4)*(w/4);
			buf[x+y*h] = sqrt(lerp(noise[target], noise[target+1], x%4/4.f) *
			lerp(noise[target], noise[target+(w/4)], y%4/4.f));
		}
	}
}

#define AddTex(x, y) m_coords[(int)x] = m_atlasCreator->add(getAssetPath("blocks", y))
#define AddTexP(i, x, y, z, d) m_coords[(int)i] = m_atlasCreator->add(x, y, z, d)
Blocks::Blocks() : m_atlas(nullptr) {
	m_atlasCreator = new AtlasCreator(64*8, 64*8); //64*((int)BlockTex::LAST/8));
	m_coords = new AtlasCreator::Coord[BlockTex::LAST];
	
	if (GlobalProperties::UseProceduralTextures) {
		uint8 *data = new uint8[64*64*4];
		uint8 *perlin = new uint8[64*64*4];
		makePerlin(64, 64, perlin);
		for (int x=0; x < 64; x++) {
			for (int y=0; y < 64; y++) {
				int32 noise; // = blend(0x292018, 0xBF9860, FastRand(0, 255));
				if (x == 0 || x == 63 || y == 0 || y == 63) {
					noise = 0x1B120B;
				} else {
					/*switch (FastRand(0, 4)) {
						case 0: noise = 0x292018; break;
						case 1: noise = 0x593F28; break;
						case 2: noise = 0x87633E; break;
						case 3: noise = 0xBF9860; break;
					}*/
					/*if (perlin[x+y*64] < 64)
						noise = 0x292018;
					else if (perlin[x+y*64] < 128)
						noise = 0x593F28;
					else if (perlin[x+y*64] < 192)
						noise = 0x87633E;
					else
						noise = 0xBF9860;*/
					noise = ((int)perlin[x+y*64] << 16) + ((int)perlin[x+y*64] << 8) + (int)perlin[x+y*64];
				}
				data[0 + x*4 + y*64*4] = noise >> 16 & 0xFF;
				data[1 + x*4 + y*64*4] = noise >> 8 & 0xFF;
				data[2 + x*4 + y*64*4] = noise & 0xFF;
				data[3 + x*4 + y*64*4] = 255;
			}
		}
		AddTexP(BlockTex::Dirt, 64, 64, 4, data);
		delete[] data;
		delete[] perlin;
	} else {
		AddTex(BlockTex::Dirt, "tex_block_dirt.png");}
		AddTex(BlockTex::DirtSign, "tex_block_dirt_sign.png");
		AddTex(BlockTex::Rock, "tex_block_rock.png");
		AddTex(BlockTex::Ore, "tex_block_ore.png");
		AddTex(BlockTex::Gold, "tex_block_silver.png");
		AddTex(BlockTex::Diamond, "tex_block_diamond.png");
		AddTex(BlockTex::HomeRed, "tex_block_home_red.png");
		AddTex(BlockTex::HomeBlue, "tex_block_home_blue.png");
		AddTex(BlockTex::SolidRed, "tex_block_red.png");
		AddTex(BlockTex::SolidBlue, "tex_block_blue.png");
		AddTex(BlockTex::Ladder, "tex_block_ladder.png");
		AddTex(BlockTex::LadderTop, "tex_block_ladder_top.png");
		AddTex(BlockTex::Spikes, "tex_block_spikes.png");
		AddTex(BlockTex::Jump, "tex_block_jump.png");
		AddTex(BlockTex::JumpTop, "tex_block_jump_top.png");
		AddTex(BlockTex::Explosive, "tex_block_explosive.png");
		AddTex(BlockTex::Metal, "tex_block_metal.png");
		AddTex(BlockTex::BankTopRed, "tex_block_bank_top_red.png");
		AddTex(BlockTex::BankLeftRed, "tex_block_bank_left_red.png");
		AddTex(BlockTex::BankFrontRed, "tex_block_bank_front_red.png");
		AddTex(BlockTex::BankRightRed, "tex_block_bank_right_red.png");
		AddTex(BlockTex::BankBackRed, "tex_block_bank_back_red.png");
		AddTex(BlockTex::BankTopBlue, "tex_block_bank_top_blue.png");
		AddTex(BlockTex::BankLeftBlue, "tex_block_bank_left_blue.png");
		AddTex(BlockTex::BankFrontBlue, "tex_block_bank_front_blue.png");
		AddTex(BlockTex::BankRightBlue, "tex_block_bank_right_blue.png");
		AddTex(BlockTex::BankBackBlue, "tex_block_bank_back_blue.png");
		AddTex(BlockTex::TeleSideA, "tex_block_teleporter_a.png");
		AddTex(BlockTex::TeleSideB, "tex_block_teleporter_b.png");
		AddTex(BlockTex::TeleTop, "tex_block_teleporter_top.png");
		AddTex(BlockTex::TeleBottom, "tex_block_teleporter_bottom.png");
		AddTex(BlockTex::Lava, "tex_block_lava.png");
		AddTex(BlockTex::Road, "tex_block_road_orig.png");
		AddTex(BlockTex::RoadTop, "tex_block_road_top.png");
		AddTex(BlockTex::RoadBottom, "tex_block_road_bottom.png");
		AddTex(BlockTex::BeaconRed, "tex_block_beacon_top_red.png");
		AddTex(BlockTex::BeaconBlue, "tex_block_beacon_top_blue.png");
		AddTex(BlockTex::TransRed, "tex_block_trans_red.png");
		AddTex(BlockTex::TransBlue, "tex_block_trans_blue.png");
	
	
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