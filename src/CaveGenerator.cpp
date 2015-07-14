#include "CaveGenerator.hpp"
#include <climits>
#include <thread>
#include "content/Registry.hpp"
#include "Chunk.hpp"
#include "Platform.hpp"
#include <stb_perlin.h>

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
#if 0
static void PaintAtPoint(World &sc, int x, int y, int z, int paintRadius, BlockType paintValue) {
	int prsq = paintRadius*paintRadius;
	for (int dx = -paintRadius; dx <= paintRadius; dx++) {
		int dxsq = dx*dx;
		for (int dy = -paintRadius; dy <= paintRadius; dy++) {
			int dxdysq = dxsq+dy*dy;
			for (int dz = -paintRadius; dz <= paintRadius; dz++)
				if (x+dx >= 0 && y+dy>= 0 && z+dz >= 0 && x+dx < sc.getChunksX()*CX && y+dy < sc.getChunksY()*CY && z+dz < sc.getChunksZ()*CZ)
					if (dxdysq+dz*dz<prsq)
						sc.set(x + dx, y + dy, z + dz, paintValue);
		}
	}
}

static void WalkL(World &sc, int x, int y, int z, int len, int paintRadius, BlockType paintValue, bool dontStopAtEdge,
	bool cond = false, BlockType condType = BlockType::Dirt) {
	const int xs = sc.getChunksX()*CX, ys = sc.getChunksY()*CY, zs = sc.getChunksZ()*CZ;
	int m = len, count = 0;

	while (count < m) {
		if (!cond || (cond && sc.get(x, y, z) == condType))
			PaintAtPoint(sc, x, y, z, paintRadius, paintValue);
		int dx = FastRand(-paintRadius, paintRadius);
		int dy = FastRand(-paintRadius, paintRadius);
		int dz = FastRand(-paintRadius, paintRadius);

		x += dx;
		y += dy;
		z += dz;

		++count;
		if (x < 0 || y < 0 || z < 0 || x >= xs || y >= ys || z >= zs) {
			if (dontStopAtEdge) {
				if (x < 0) x = 0;
				if (y < 0) y = 0;
				if (z < 0) z = 0;
				if (x >= xs) x = xs - 1;
				if (y >= ys) y = ys - 1;
				if (z >= zs) z = zs - 1;
			} else
				break;
		}

		if (FastRand() & 0x800) {
			if (!cond || (cond && sc.get(x, y, z) == condType))
				PaintAtPoint(sc, x, y, z, paintRadius, paintValue);
			x += dx;
			y += dy;
			z += dz;

			++count;
			if (x < 0 || y < 0 || z < 0 || x >= xs || y >= ys || z >= zs) {
				if (dontStopAtEdge) {
					if (x < 0) x = 0;
					if (y < 0) y = 0;
					if (z < 0) z = 0;
					if (x >= xs) x = xs - 1;
					if (y >= ys) y = ys - 1;
					if (z >= zs) z = zs - 1;
				} else
					break;
			}
		}  
	}
}
static void Walk(World &sc, double len, int paintRadius, BlockType paintValue, bool dontStopAtEdge,
	bool cond = false, BlockType condType = BlockType::Dirt) {
	const int xs = sc.getChunksX()*CX, ys = sc.getChunksY()*CY, zs = sc.getChunksZ()*CZ;
	WalkL(sc, FastRand(0, xs-1), FastRand(0, ys-1), FastRand(0, zs-1), xs*ys*zs*len, paintRadius, paintValue, dontStopAtEdge,
		cond, condType);
}

static void AddRocks(World &sc, const CaveGenerator::GenConf &gc) {
	int xs = sc.getChunksX()*CX, ys = sc.getChunksY()*CY, zs = sc.getChunksZ()*CZ;
	int size = xs*zs;
	int numRocks = FastRand(gc.rocks.freq*size, (gc.rocks.freq+gc.rocks.freqAmpl)*size);
	for (int i = 0; i < numRocks; i++) {
		int x = FastRand(0, xs);
		int z = FastRand(0, zs);

		// generate a random y-value weighted toward a deep depth
		float yf = 0;
		for (int j = 0; j < 4; j++)
			yf += FastRandF();
		yf /= 2;
		yf = 1 - fabs(yf - 1);
		int y = (int)(yf * ys)-ys+4;
		// +4: little padding so we can see the rockballs

		int rockSize = (int)(FastRandF()*(gc.rocks.maxSize-gc.rocks.minSize)+gc.rocks.minSize);

		PaintAtPoint(sc, x, y, z, rockSize, BlockType::Rock);
	}
}

static void AddLava(World &sc, const CaveGenerator::GenConf &gc) {
	const int xs = sc.getChunksX()*CX, ys = sc.getChunksY()*CY, zs = sc.getChunksZ()*CZ;
	int minY = gc.lava.minLevel*ys, maxY = gc.lava.maxLevel*ys,
		dY = maxY-minY;
	int minNum = gc.lava.freq * xs*dY*zs, maxNum = (gc.lava.freq+gc.lava.freqAmpl) * xs*dY*zs;
	int numFlows = FastRandF()*(maxNum-minNum)+minNum;
	while (numFlows > 0) {
		int x = FastRand(0, xs);
		int z = FastRand(2, zs);
		int y = FastRandF()*(maxY-minY)+minY;

		if (sc.get(x, y, z) == BlockType::Air) {
			sc.set(x, y, z, BlockType::Lava);
			numFlows--;
		}
	}
}

static void AddOre(Chunk &c, const CaveGenerator::GenConf &gc) {
	int yRand = FastRand(0xFFFF);
	for (int x = 0; x < CX; x++)
		for (int y = 0; y < CY; y++)
			for (int z = 0; z < CZ; z++) {
				float noise = stb_perlin_noise3(x/8.f, y/8.f+yRand, z/8.f);
				c.set(x, y, z, (noise > gc.ore.thresold) ? BlockType::Ore : BlockType::Dirt);
			}
}

static void AddDiamond(World &sc, const CaveGenerator::GenConf &gc) {
	const int xs = sc.getChunksX()*CX, ys = sc.getChunksY()*CY, zs = sc.getChunksZ()*CZ;
	int minY = gc.diamond.minLevel*ys, maxY = gc.diamond.maxLevel*ys,
		dY = maxY-minY;
	int minNum = gc.diamond.freq * xs*dY*zs, maxNum = (gc.diamond.freq+gc.diamond.freqAmpl) * xs*dY*zs;
	int numDiam = FastRandF()*(maxNum-minNum)+minNum;
	while (numDiam > 0) {
		int x = FastRand(0, xs);
		int z = FastRand(0, zs);
		int y = FastRandF()*(maxY-minY)+minY;

		if (y > 2 && (sc.get(x, y, z) == BlockType::Dirt || sc.get(x, y, z) == BlockType::Ore)) {
			sc.set(x, y, z, BlockType::Diamond);
			numDiam--;
		}
	}
}

static void AddGold(World &sc, const CaveGenerator::GenConf &gc) {
	const int xs = sc.getChunksX()*CX, ys = sc.getChunksY()*CY, zs = sc.getChunksZ()*CZ;
	int minY = gc.gold.minLevel*ys, maxY = gc.gold.maxLevel*ys,
		dY = maxY-minY;
	int minNum = gc.gold.freq * xs*dY*zs, maxNum = (gc.gold.freq+gc.gold.freqAmpl) * xs*dY*zs;
	int numVeins = FastRandF()*(maxNum-minNum)+minNum;
	while (numVeins > 0) {
		int x = FastRand(0, xs);
		int z = FastRand(0, zs);
		int y = FastRandF()*(maxY-minY)+minY;

		if (y > 2 && (sc.get(x, y, z) == BlockType::Dirt || sc.get(x, y, z) == BlockType::Ore)) {
			WalkL(sc, x, y, z, FastRand(gc.gold.minSize, gc.gold.maxSize), 1, BlockType::Gold, false, true);
			numVeins--;
		}
	}
}
#endif
void CaveGenerator::Generate(WorldRef wr, const GenConf &gc, ChunkRef cr) {
	World &W = *wr;
	Chunk &c = *cr;
	/*const int xs = sc.getChunksX()*CX, ys = sc.getChunksY()*CY, zs = sc.getChunksZ()*CZ;
	int groundLevel = ys*gc.groundLevel;*/
	FastRandSeed(gc.seed);

	c.state = Chunk::State::Generating;

	int groundLevel = 0;
	for (int y = 0; y < CY; y++)
		for (int x = 0; x < CX; x++)
			for (int z = 0; z < CZ; z++)
				c.setBlock(x, y, z, y < 8 ? Content::BlockDefaultId : Content::BlockAirId);

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