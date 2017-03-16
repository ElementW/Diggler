#include "CaveGenerator.hpp"
#include <climits>
#include <thread>
#include "content/Registry.hpp"
#include "Chunk.hpp"
#include "Platform.hpp"
#include <simplexnoise.h>

namespace Diggler {

CaveGenerator::GenConf::GenConf() :
  groundLevel(0.9), oreFactor(0.5) {
  seed = FastRand();

  cave.enabled = true;
  cave.length = 0.0002;
  cave.count = 18;

  ore.enabled = true;
  ore.thresold = 0.3;

  lava.enabled = true;
  lava.freq = 0.0001;
  lava.freqAmpl = 0;
  lava.minLevel = 0;
  lava.maxLevel = 0.5;

  diamond.enabled = true;
  diamond.freq = 0.0005;
  diamond.freqAmpl = 0;
  diamond.minLevel = 0;
  diamond.maxLevel = 0.5;

  rocks.enabled = true;
  rocks.freq = 0.05;
  rocks.freqAmpl = 0;
  rocks.minLevel = 0;
  rocks.maxLevel = 0.1;
  rocks.minSize = 2;
  rocks.maxSize = 4;

  gold.enabled = true;
  gold.freq = 0.002;
  gold.freqAmpl = 0;
  gold.minLevel = 0;
  gold.maxLevel = 0.7;
  gold.minSize = 2;
  gold.maxSize = 5;
}

void CaveGenerator::Generate(WorldRef wr, const GenConf &gc, ChunkRef cr) {
  World &W = *wr;
  Chunk &c = *cr;
  /*const int xs = sc.getChunksX()*CX, ys = sc.getChunksY()*CY, zs = sc.getChunksZ()*CZ;
  int groundLevel = ys*gc.groundLevel;*/
  FastRandSeed(gc.seed);

  c.state = Chunk::State::Generating;

  /*
  int groundLevel = 0;
  for (int y = 0; y < CY; ++y)
    for (int x = 0; x < CX; ++x)
      for (int z = 0; z < CZ; ++z)
        c.setBlock(x, y, z, y < 8 ? Content::BlockUnknownId : Content::BlockAirId);
  for (int y = CY; y > 8; --y)
    for (int x = 0; x < CX; ++x) {
      c.setBlock(x, y, 0, Content::BlockUnknownId);
      c.setBlock(x, y, CZ-1, Content::BlockUnknownId);
    }*/

  constexpr auto CX = Chunk::CX, CY = Chunk::CY, CZ = Chunk::CZ;
  const glm::ivec3 cp = c.getWorldChunkPos() * glm::ivec3(CX, CY, CZ);
  for (int ly = 0; ly < CY; ++ly) {
    int y = cp.y + ly;
    for (int lx = 0; lx < CX; ++lx) {
      int x = cp.x + lx;
      for (int lz = 0; lz < CZ; ++lz) {
        int z = cp.z + lz;
        if (y >= -8) {
          c.setBlock(lx, ly, lz, y < raw_noise_3d(x/16.f, 0, z/16.f)*8 ? Content::BlockUnknownId : Content::BlockAirId);
        } else {
          c.setBlock(lx, ly, lz, raw_noise_3d(x/16.f, y/16.f, z/16.f) > 0.7 ? Content::BlockAirId : Content::BlockUnknownId);
        }
      }
    }
  }
#if 0
  if (gc.ore.enabled)
    AddOre(*c, gc);
  else
    for (int y = Y*CY; y < groundLevel; y++)
      for (int x = 0; x < CX; x++)
        for (int z = 0; z < CZ; z++)
          c->set(x, y, z, BlockType::Dirt);

  // Mountains
  int yRand = FastRand(0xFFFF);
  for (int y = groundLevel; y < CY; y++) {
    float intensity = (groundLevel-y)/(float)(ys-groundLevel)+1;
    for (int x = 0; x < CX; x++)
      for (int z = 0; z < CZ; z++) {
        float noise = stb_perlin_noise3(x/8.f, y/8.f+yRand, z/8.f) * intensity;
        sc.set(x, y, z, (noise > 0.05f) ? BlockType::Dirt : BlockType::Air);
      }
  }

  if (gc.cave.enabled) {
    for (int i=0; i < gc.cave.count; ++i)
      Walk(sc, gc.cave.length, 3, BlockType::Air, true);
  }

  if (gc.lava.enabled)
    AddLava(sc, gc);
  if (gc.rocks.enabled)
    AddRocks(sc, gc);
  if (gc.diamond.enabled)
    AddDiamond(sc, gc);
  if (gc.gold.enabled)
    AddGold(sc, gc);
#endif

  c.state = Chunk::State::Ready;
}

}
