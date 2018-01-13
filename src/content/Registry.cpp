#include "Registry.hpp"

#include "../GlobalProperties.hpp"
#include "../util/Log.hpp"
#include "Content.hpp"

#define PRINT_BLOCK_REGISTRATIONS 1

namespace diggler {
namespace content {

using Util::Log;
using namespace Util::Logging::LogLevels;

static const char *TAG = "Registry";

Registry::BlockRegistration::BlockRegistration(Registry &registry,
  const Registry::BlockNameMap::iterator &it) :
  registry(registry),
  it(it),
  state(Uncommitted),
  def(it->second->second) {
}

Registry::BlockRegistration::~BlockRegistration() {
  if (state == Uncommitted) {
    registry.m_blocks.erase(it->second);
  }
}

Registry::BlockRegistration::BlockRegistration(Registry::BlockRegistration &&o) :
  registry(o.registry),
  it(o.it),
  state(o.state),
  def(o.def) {
  o.state = Moved;
}

BlockId Registry::BlockRegistration::commit() {
  state = Committed;
#if PRINT_BLOCK_REGISTRATIONS
  Log(Info, TAG) << "Registered block " << it->first << " with id " << it->second->first;
#endif
  return it->second->first;
}

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

bool Registry::isTransparent(BlockId id) const {
  if (id == content::BlockAirId)
    return true;
  return false;
  // TODO return getBlockDef(id).isTransparent;
}

bool Registry::isFaceVisible(BlockId id1, BlockId id2) const {
  // TODO: node mesh/boxes -> not fullblock, faces may not be hidden
  if (isTransparent(id1)) {
    return (id1 != id2);
  } else {
    return isTransparent(id2);
  }
}

bool Registry::canEntityGoThrough(BlockId id/* , Entity& ent*/) const {
  if (id == content::BlockAirId)
    return true;
  return false;
  /*return (t == BlockType::TransRed && team == Player::Team::Red) ||
       (t == BlockType::TransBlue && team == Player::Team::Blue);*/
}

using Coord = Util::TexturePacker::Coord;
static Coord unk1, unk2, unk3, unk4, unk5, unk6, unk7, unk8;
#define AddTex(b, t) Coord b = m_texturePacker->add(getAssetPath("blocks", t));
Registry::Registry(Game &G) :
  m_atlas(nullptr),
  m_nextMaxBlockId(content::BlockUnknownId + 1) {
  { Registry::BlockRegistration br(registerBlock(content::BlockAirId, "air"));
    br.def.appearance.look.type = BlockDef::Appearance::Look::Type::Hidden;
    br.def.phys.hasCollision = false;
    br.commit();
  }
  { Registry::BlockRegistration br(registerBlock(content::BlockUnknownId, "unknown"));
    br.def.appearance.look.type = BlockDef::Appearance::Look::Type::Hidden;
    br.def.phys.hasCollision = true;
    br.commit();
  }

  if (GlobalProperties::IsClient) {
    m_texturePacker = new Util::TexturePacker(G, 64*8, 64*8);
    m_texturePacker->freezeTexUpdate(true);

    // Valve checkerboard! :)
    unk1 = m_texturePacker->add(2, 2, 3, (uint8*)"\x00\x00\x00\xFF\x00\xFF\xFF\x00\xFF\x00\x00\x00");
    unk2 = m_texturePacker->add(2, 2, 3, (uint8*)"\x00\x00\x00\x00\x00\xFF\x00\x00\xFF\x00\x00\x00");
    unk3 = m_texturePacker->add(2, 2, 3, (uint8*)"\x00\x00\x00\x00\xFF\x00\x00\xFF\x00\x00\x00\x00");
    unk4 = m_texturePacker->add(2, 2, 3, (uint8*)"\x00\x00\x00\xFF\x00\x00\xFF\x00\x00\x00\x00\x00");
    unk5 = m_texturePacker->add(2, 2, 3, (uint8*)"\x00\x00\x00\x00\xFF\xFF\x00\xFF\xFF\x00\x00\x00");
    unk6 = m_texturePacker->add(2, 2, 3, (uint8*)"\x00\x00\x00\xFF\xFF\x00\xFF\xFF\x00\x00\x00\x00");
    unk7 = m_texturePacker->add(2, 2, 3, (uint8*)"\x00\x00\x00\xFF\xFF\xFF\xFF\xFF\xFF\x00\x00\x00");
    unk8 = m_texturePacker->add(2, 2, 3, (uint8*)"\x00\x00\x00\x00\x0F\x00\x00\x0F\x00\x00\x00\x00");

    m_texturePacker->freezeTexUpdate(false);
    m_atlas = m_texturePacker->getAtlas();
  }
}

Registry::~Registry() {
  if (GlobalProperties::IsClient) {
    delete m_texturePacker;
  }
}

Util::TexturePacker::Coord Registry::addTexture(const std::string &texName,
  const std::string &path) {
  const Util::TexturePacker::Coord coord = m_texturePacker->add(path);
  m_textureCoords.emplace(std::piecewise_construct,
    std::forward_as_tuple(texName),
    std::forward_as_tuple(coord));
  return coord;
}

const Util::TexturePacker::Coord* Registry::blockTexCoord(BlockId t, FaceDirection d,
  const glm::ivec3 &pos) const {
  if (t == content::BlockUnknownId) {
    const Coord *unk[] = {
      &unk1, &unk2, &unk3, &unk4, &unk5, &unk6, &unk7, &unk8
    };
    return unk[rmod(pos.x, 2) + 2*(rmod(pos.y, 2)) + 4*(rmod(pos.z, 2))]; //&m_unknownBlockTex;
  }
  const BlockDef *try_bdef;
  try {
    try_bdef = &m_blocks.at(t);
  } catch (const std::out_of_range&) {
    return &unk1;
  }
  const BlockDef &bdef = *try_bdef;
  using Type = BlockDef::Appearance::Look::Type;
  switch (bdef.appearance.look.type) {
    case Type::Cube: {
      const BlockDef::Appearance::Texture &tex =
        bdef.appearance.look.data.cube.sides[static_cast<uint>(d)].texture->second;
      if (tex.repeat.xdiv == 1 && tex.repeat.ydiv == 1) {
        return &tex.coord;
      }
      size_t idx = 0;
      switch (d) {
      case FaceDirection::XInc:
        idx = rmod(pos.z, tex.repeat.xdiv) +
              tex.repeat.xdiv*(rmod(pos.y, tex.repeat.ydiv));
        break;
      case FaceDirection::XDec:
        idx = rmod(-pos.z - 1, tex.repeat.xdiv) +
              tex.repeat.xdiv*(rmod(pos.y, tex.repeat.ydiv));
        break;
      case FaceDirection::YInc:
        idx = rmod(-pos.z - 1, tex.repeat.xdiv) +
              tex.repeat.xdiv*(rmod(pos.x, tex.repeat.ydiv));
        break;
      case FaceDirection::YDec:
        idx = rmod(pos.z, tex.repeat.xdiv) +
              tex.repeat.xdiv*(rmod(pos.x, tex.repeat.ydiv));
        break;
      case FaceDirection::ZInc:
        idx = rmod(-pos.x - 1, tex.repeat.xdiv) +
              tex.repeat.xdiv*(rmod(pos.y, tex.repeat.ydiv));
        break;
      case FaceDirection::ZDec:
        idx = rmod(pos.x, tex.repeat.xdiv) +
              tex.repeat.xdiv*(rmod(pos.y, tex.repeat.ydiv));
        break;
      }
      return &tex.divCoords[idx];
    } break;
    case Type::Hidden:
      ;
  }
  return nullptr;
}

Registry::BlockRegistration Registry::registerBlock(BlockId id, const char *name) {
  BlockIdMap::iterator bit = m_blocks.emplace(std::piecewise_construct,
     std::forward_as_tuple(id),
     std::forward_as_tuple())
   .first;
  return BlockRegistration(*this, m_blockNames.emplace(std::piecewise_construct,
      std::forward_as_tuple(name),
      std::forward_as_tuple(bit))
    .first);
}

Registry::BlockRegistration Registry::registerBlock(const char *name) {
  BlockId id = content::BlockUnknownId;
  if (m_freedBlockIds.empty()) {
    id = m_nextMaxBlockId;
    ++m_nextMaxBlockId;
  } else {
    id = m_freedBlockIds.back();
    m_freedBlockIds.pop_back();
  }
  return registerBlock(id, name);
}

}
}
