#include "Registry.hpp"
#include "Content.hpp"
#include "../GlobalProperties.hpp"

namespace Diggler {

#define NON 0x0
#define RED 0x1
#define BLU 0x2
#define ANY (RED | BLU)

struct DefBlocksInfo {
  const char *id, *name;
  int moneyValue, oreValue, oreCost;
  int team;
  const char *buildergunIcon;
};

static const DefBlocksInfo DefBlocksInfos[] = {
  {"diggler:air", "Air",					0,		0,	0,		NON, "deconstruction.png"},
  {"diggler:dirt", "Dirt",				0,		0,	0,		NON, nullptr},
  {"diggler:ore", "Ore",					0,		25,	0,		NON, nullptr},
  {"diggler:gold", "Gold",				100,	0,	0,		NON, nullptr},
  {"diggler:diamond", "Diamond",			1000,	0,	0,		NON, nullptr},
  {"diggler:rock", "Rock",				0,		0,	0,		NON, nullptr},
  {"diggler:ladder", "Ladder",			0,		0,	25,		ANY, "ladder.png"},
  {"diggler:explosive", "Explosives",		0,		0,	100,	ANY, "explosive.png"},
  {"diggler:jump", "Jump pad",			0,		0,	25,		ANY, "jump.png"},
  {"diggler:shock", "Shock Block",		0,		0,	50,		ANY, "spikes.png"},
  {"diggler:bank_red", "Bank",			0,		0,	50,		RED, "bank_red.png"},
  {"diggler:bank_blue", "Bank",			0,		0,	50,		BLU, "bank_blue.png"},
  {"diggler:beacon_red", "Beacon",		0,		0,	50,		RED, "beacon.png"},
  {"diggler:beacon_blue", "Beacon",		0,		0,	50,		BLU, "beacon.png"},
  {"diggler:road", "Road",				0,		0,	10,		ANY, "road.png"},
  {"diggler:solid_red", "Solid Block",	0,		0,	10,		RED, "solid_red.png"},
  {"diggler:solid_blue", "Solid Block",	0,		0,	10,		BLU, "solid_blue.png"},
  {"diggler:metal", "Metal Block",		0,		0,	0,		NON, "metal.png"},
  {"diggler:dirt_sign", "Dirt",			0,		0,	0,		NON, nullptr},
  {"diggler:lava", "Lava",				0,		0,	0,		NON, nullptr},
  {"diggler:transp_red", "Force Field",	0,		0,	25,		ANY, "translucent_red.png"},
  {"diggler:transp_blue", "Force Field",	0,		0,	25,		ANY, "translucent_blue.png"}
};

bool ContentRegistry::isTransparent(BlockId id) {
  if (id == Content::BlockAirId)
    return true;
  return false;
  // TODO return getBlockDef(id).isTransparent;
}

bool ContentRegistry::isFaceVisible(BlockId id1, BlockId id2) {
  // TODO: node mesh/boxes -> not fullblock, faces may not be hidden
  if (isTransparent(id1)) {
    return (id1 != id2);
  } else {
    return isTransparent(id2);
  }
}

bool ContentRegistry::canEntityGoThrough(BlockId id/* , Entity& ent*/) {
  if (id == Content::BlockAirId)
    return true;
  return false;
  /*return (t == BlockType::TransRed && team == Player::Team::Red) ||
       (t == BlockType::TransBlue && team == Player::Team::Blue);*/
}

using Coord = TexturePacker::Coord;
#define AddTex(b, t) Coord b = m_texturePacker->add(getAssetPath("blocks", t));
ContentRegistry::ContentRegistry() : m_atlas(nullptr) {
  m_texturePacker = new TexturePacker(64*8, 64*8);
  m_texturePacker->freezeTexUpdate(true);

  // Valve checkerboard! :)
  m_unknownBlockTex = m_texturePacker->add(2, 2, 3, (uint8*)"\x00\x00\x00\xFF\x00\xFF\xFF\x00\xFF\x00\x00\x00");
#if 1
  AddTex(texDirt,				"tex_block_dirt.png");
  AddTex(texDirtSign,			"tex_block_dirt_sign.png");
  AddTex(texRock,				"tex_block_rock.png");
  AddTex(texOre,				"tex_block_ore.png");
  AddTex(texGold,				"tex_block_silver.png");
  AddTex(texDiamond,			"tex_block_diamond.png");
  AddTex(texHomeRed,			"tex_block_home_red.png");
  AddTex(texHomeBlue,			"tex_block_home_blue.png");
  AddTex(texSolidRed,			"tex_block_red.png");
  AddTex(texSolidBlue,		"tex_block_blue.png");
  AddTex(texLadder,			"tex_block_ladder.png");
  AddTex(texLadderTop,		"tex_block_ladder_top.png");
  AddTex(texSpikes,			"tex_block_spikes.png");
  AddTex(texJump,				"tex_block_jump.png");
  AddTex(texJumpTop,			"tex_block_jump_top.png");
  AddTex(texExplosive,		"tex_block_explosive.png");
  AddTex(texMetal,			"tex_block_metal.png");
  AddTex(texBankTopRed,		"tex_block_bank_top_red.png");
  AddTex(texBankLeftRed,		"tex_block_bank_left_red.png");
  AddTex(texBankFrontRed,		"tex_block_bank_front_red.png");
  AddTex(texBankRightRed,		"tex_block_bank_right_red.png");
  AddTex(texBankBackRed,		"tex_block_bank_back_red.png");
  AddTex(texBankTopBlue,		"tex_block_bank_top_blue.png");
  AddTex(texBankLeftBlue,		"tex_block_bank_left_blue.png");
  AddTex(texBankFrontBlue,	"tex_block_bank_front_blue.png");
  AddTex(texBankRightBlue,	"tex_block_bank_right_blue.png");
  AddTex(texBankBackBlue,		"tex_block_bank_back_blue.png");
  AddTex(texTeleSideA,		"tex_block_teleporter_a.png");
  AddTex(texTeleSideB,		"tex_block_teleporter_b.png");
  AddTex(texTeleTop,			"tex_block_teleporter_top.png");
  AddTex(texTeleBottom,		"tex_block_teleporter_bottom.png");
  AddTex(texLava,				"tex_block_lava.png");
  AddTex(texRoad,				"tex_block_road_orig.png");
  AddTex(texRoadTop,			"tex_block_road_top.png");
  AddTex(texRoadBottom,		"tex_block_road_bottom.png");
  AddTex(texBeaconRed,		"tex_block_beacon_top_red.png");
  AddTex(texBeaconBlue,		"tex_block_beacon_top_blue.png");
  AddTex(texTransRed,			"tex_block_trans_red.png");
  AddTex(texTransBlue,		"tex_block_trans_blue.png");

  Coord texNil {0, 0, 0, 0};

  const Coord sideTextures[22][6] = {
    /* Air */	{texNil, texNil, texNil, texNil, texNil, texNil},
    /* Dirt */	{texDirt, texDirt, texDirt, texDirt, texDirt, texDirt},
    /* Ore */	{texOre, texOre, texOre, texOre, texOre, texOre},
    /* Gold */	{texGold, texGold, texGold, texGold, texGold, texGold},
    /*Diamond*/	{texDiamond, texDiamond, texDiamond, texDiamond, texDiamond, texDiamond},
    /* Rock */	{texRock, texRock, texRock, texRock, texRock, texRock},
    /* Ladder */{texLadder, texLadder, texLadderTop, texLadderTop, texLadder, texLadder},
    /* TNT*/	{texExplosive, texExplosive, texExplosive, texExplosive, texExplosive, texExplosive},
    /* Jump */	{texJump, texJump, texJumpTop, texTeleBottom, texJump, texJump},
    /* Shock */	{texTeleSideA, texTeleSideA, texTeleBottom, texSpikes, texTeleSideB, texTeleSideB},
    /*BankRed*/	{texBankFrontRed, texBankBackRed, texBankTopRed, texBankTopRed, texBankLeftRed, texBankRightRed},
    /*BankBlue*/{texBankFrontBlue, texBankBackBlue, texBankTopBlue, texBankTopBlue, texBankLeftBlue, texBankRightBlue},
    /*BeaconR*/	{texTeleSideA, texTeleSideA, texBeaconRed, texLadderTop, texTeleSideB, texTeleSideB},
    /*BeaconB*/	{texTeleSideA, texTeleSideA, texBeaconBlue, texLadderTop, texTeleSideB, texTeleSideB},
    /* Road */	{texRoad, texRoad, texRoad, texRoad, texRoad, texRoad},
    /* SolidR */{texSolidRed, texSolidRed, texSolidRed, texSolidRed, texSolidRed, texSolidRed},
    /* SolidB */{texSolidBlue, texSolidBlue, texSolidBlue, texSolidBlue, texSolidBlue, texSolidBlue},
    /* Metal */	{texMetal, texMetal, texMetal, texMetal, texMetal, texMetal},
    /*DirtSign*/{texDirtSign, texDirtSign, texDirtSign, texDirtSign, texDirtSign, texDirtSign},
    /* Lava */	{texLava, texLava, texLava, texLava, texLava, texLava},
    /* TransR */{texTransRed, texTransRed, texTransRed, texTransRed, texTransRed, texTransRed},
    /* TransB */{texTransBlue, texTransBlue, texTransBlue, texTransBlue, texTransBlue, texTransBlue},
  };
  for (int i=0; i < 22; ++i) {
    BlockFaceTexCoords c;
    for (int j=0; j < 6; ++j)
      c.coords[j] = sideTextures[i][j];
    m_coords.push_back(c);
  }
#endif

  m_texturePacker->freezeTexUpdate(false);
  m_atlas = m_texturePacker->getAtlas();
}

ContentRegistry::~ContentRegistry() {
  delete m_texturePacker;
}

const TexturePacker::Coord* ContentRegistry::gTC(BlockId t, FaceDirection d) const {
  if (t == Content::BlockUnknownId) {
    return &m_unknownBlockTex;
  }
  return &(m_coords[(int)t].coords[(int)d]);
}

const Texture* ContentRegistry::getAtlas() const {
  return m_atlas;
}

}
