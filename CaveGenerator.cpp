#include "CaveGenerator.hpp"
#include "Blocks.hpp"
#include "Chunk.hpp"
#include <climits>
#include <thread>
#include "Platform.hpp"

namespace Diggler {

std::string CaveGenerator::CaveInfo = "";
int CaveGenerator::GROUND_LEVEL = 0;

// Generates a set of constant values.
void CaveGenerator::GenerateConstant(Superchunk &sc, BlockType value) {
	for (int x = 0; x < sc.getChunksX()*CX; x++)
		for (int y = 0; y < sc.getChunksY()*CY; y++)
			for (int z = 0; z < sc.getChunksZ()*CZ; z++)
				sc.set(x, y, z, value);
}

void CaveGenerator::GenerateConstant(Arr3BT &data, BlockType value) {
	for (int x = 0; x < data.getX(); x++)
		for (int y = 0; y < data.getY(); y++)
			for (int z = 0; z < data.getZ(); z++)
				data[x][y][z] = value;
}

// Create a cave system.
void CaveGenerator::GenerateCaveSystem(Superchunk &sc, bool includeLava, uint oreFactor) {
	float gradientStrength = FastRand(0, 10000)/100000.f;
	getDebugStream() << gradientStrength << std::endl;
	const int xs = sc.getChunksX()*CX, ys = sc.getChunksY()*CY, zs = sc.getChunksZ()*CZ;
	GROUND_LEVEL = ys*7/8;
	int size = cbrt(xs*ys*zs);
	
	//GenerateConstant(sc, BlockType::Dirt);
	for (int x = 0; x < xs; x++)
		for (int y = 0; y < ys; y++)
			for (int z = 0; z < zs; z++)
				sc.set(x, y, z, BlockType::Dirt);

	// Add ore.
	Arr3f *oreNoise = new Arr3f(xs/2, ys/2, zs/2);
	GeneratePerlinNoise(*oreNoise);
	
	Arr3f *interpOreNoise = InterpolateData(*oreNoise, 2);
	for (uint i = 0; i < oreFactor; i++)
		PaintWithRandomWalk(sc, *interpOreNoise, 1, BlockType::Ore, false);

	delete oreNoise; delete interpOreNoise;

	// Add minerals.
	AddGold(sc);
	AddDiamond(sc);

	// Level off everything above ground level, replacing it with mountains.
	Arr3f *mountainNoiseBase = new Arr3f(xs/2, ys/2, zs/2);
	GeneratePerlinNoise(*mountainNoiseBase);
	Arr3f *mountainNoise = InterpolateData(*mountainNoiseBase, 2);
	delete mountainNoiseBase;
	/*for (int y = GROUND_LEVEL; y < ys; y++)
		getDebugStream() << y << '=' << std::max(0.f, 1.f-(y-GROUND_LEVEL)/(float)(ys-GROUND_LEVEL)) << std::endl;*/
	for (int x = 0; x < xs; x++)
		for (int y = GROUND_LEVEL; y < ys; y++)
			for (int z = 0; z < zs; z++)
				(*mountainNoise)[x][y][z] = std::max(0.f, 1.f-(y-GROUND_LEVEL)/(float)(ys-GROUND_LEVEL));

	Arr3f *gradient = GenerateGradient(xs, ys, zs);
	AddDataTo(*mountainNoise, *gradient, 0.1f, 0.9f);
	delete gradient;
	Arr3BT *mountainData = new Arr3BT(xs, ys, zs);
	GenerateConstant(*mountainData, BlockType::Air);
	int numMountains = FastRand(size, size * 3);
	for (int i = 0; i < numMountains; i++)
		PaintWithRandomWalk(*mountainData, *mountainNoise, FastRand(2, 3), BlockType::Dirt, false);
	for (int x = 0; x < xs; x++)
		for (int y = ys-1; y >= GROUND_LEVEL; y--)
			for (int z = 0; z < zs; z++)
				if ((*mountainData)[x][y][z] == BlockType::Air)
					sc.set(x, y, z, BlockType::Air);
	
	delete mountainNoise; delete mountainData;
#if 1
	// Carve some caves into the ground.
	Arr3f *caveNoiseBase = new Arr3f(xs / 2, ys / 2, zs / 2); GeneratePerlinNoise(*caveNoiseBase);
	Arr3f *caveNoise = InterpolateData(*caveNoiseBase, 2);
	delete caveNoiseBase;
	gradient = GenerateGradient(xs, ys, zs);
	AddDataTo(*caveNoise, *gradient, 1 - gradientStrength, gradientStrength);
	delete gradient;
	int cavesToCarve = 2; //FastRand(size / 8, size / 4);
	for (int i = 0; i < cavesToCarve; i++)
		PaintWithRandomWalk(sc, *caveNoise, 2, BlockType::Air, false);
	delete caveNoise;

	// Carve the map into a sphere.
	Arr3f *sphereGradient = GenerateRadialGradient(xs, ys, zs);
	cavesToCarve = 2; //FastRand(size / 8, size / 4);
	for (int i = 0; i < cavesToCarve; i++)
		PaintWithRandomWalk(sc, *sphereGradient, 2, BlockType::Air, true);
	delete sphereGradient;

	// Add rocks.
	AddRocks(sc);

	// Add lava.
	if (includeLava)
		AddLava(sc);
#endif
}


void CaveGenerator::AddRocks(Superchunk &sc) {
	int xs = sc.getChunksX()*CX, ys = sc.getChunksY()*CY, zs = sc.getChunksZ()*CZ;
	int size = cbrt(xs*ys*zs);
	int numRocks = FastRand(size, 2*size);
	CaveInfo += " numRocks=" + numRocks;
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

		int rockSize = (int)((FastRandF() + FastRandF() + FastRandF() + FastRandF()) * 2);

		PaintAtPoint(sc, x, y, z, rockSize, BlockType::Rock);
	}
}

void CaveGenerator::AddLava(Superchunk &sc) {
	const int xs = sc.getChunksX()*CX, ys = sc.getChunksY()*CY, zs = sc.getChunksZ()*CZ;
	// int numDiamonds = 16; for a 64x64x64 map
	// Changed to be more popular
	int numFlows = FastRand(cbrt(xs*ys*zs) / 16, cbrt(xs*ys*zs) / 2);
	//int numFlows = randGen.Next(size / 16, size / 2);
	while (numFlows > 0) {
		int x = FastRand(0, xs);
		int z = FastRand(0, zs);

		//switch (randGen.Next(0, 4)) {
		//    case 0: x = 0; break;
		//    case 1: x = size - 1; break;
		//    case 2: y = 0; break;
		//    case 3: y = size - 1; break;
		//}

		// generate a random y-value weighted toward a medium depth
		int y = FastRand(ys / 6, ys / 2);

		if (z > 2 && sc.get(x, y , z) == BlockType::Air) {
			//sc.set(x, y, z, BlockType::Rock);
			sc.set(x, y, z, BlockType::Lava);
			numFlows -= 1;
		}
	}
}

void CaveGenerator::AddDiamond(Superchunk &sc) {
	CaveInfo += "diamond";

	const int xs = sc.getChunksX()*CX, ys = sc.getChunksY()*CY, zs = sc.getChunksZ()*CZ;
	// int numDiamonds = 16; for a 64x64x64 map
	// Changed to be more popular
	int numDiamonds = cbrt(xs*ys*zs) / 2;
	for (int i = 0; i < numDiamonds; i++) {
		int x = FastRand(xs);
		int z = FastRand(zs);

		// generate a random y-value weighted toward a deep depth
		float yf = 0;
		for (int j = 0; j < 4; j++)
			yf += FastRandF();
		yf /= 2;
		yf = fabs(yf - 1);
		int y = (int)(yf * FastRand(ys));

		sc.set(x, y, z, BlockType::Diamond);
	}
}

// Gold appears in fairly numerous streaks, located at medium depths.
void CaveGenerator::AddGold(Superchunk &sc) {
	CaveInfo += "gold";
	int xs = sc.getChunksX()*CX, ys = sc.getChunksY()*CY, zs = sc.getChunksY()*CY;

	int numVeins = cbrt(xs * ys * zs) / 4; // 16 for 64x64x64
	for (int i = 0; i < numVeins; i++) {
		float mean = cbrt(xs * ys * zs);
		int fieldLength = FastRand(mean/3, mean);// Original: randGen.Next(size/3, size);
		float x = FastRandF() * xs;
		float z = FastRandF() * zs;
		
		// generate a random z-value weighted toward a medium depth
		float yf = 0;
		for (int j = 0; j < 4; j++)
			yf += FastRandF();
		yf /= 2;
		yf = 1 - std::abs(yf - 1);
		float y = yf * ys;

		float dx = FastRandF() * 2 - 1;
		float dy = FastRandF() * 2 - 1;
		float dz = FastRandF() * 2 - 1;
		float dl = sqrt(dx * dx + dy * dy + dz * dz);
		dx /= dl; dy /= dl; dz /= dl;

		for (int j = 0; j < fieldLength; j++) {
			x += dx;
			y += dy;
			z += dz;
			if (x >= 0 && y >= 0 && z >= 0 && x < xs && y < ys && z < zs)
				sc.set((int)x, (int)y, (int)z, BlockType::Gold);
			int tx = 0, ty = 0, tz = 0;
			switch (FastRand(0, 3)) {
			case 0:
				tx += 1;
				break;
			case 1:
				ty += 1;
				break;
			case 2:
				tz += 1;
				break;
			}
			if (x + tx >= 0 && y + ty>= 0 && z+tz >= 0 && x+tx < xs && y+ty < ys && z+tz < zs)
				sc.set((int)x+tx, (int)y+ty, (int)z+tz, BlockType::Gold);
		}
	}
}

// Generates a cube of noise with sides of length size. Noise falls in a linear
// distribution ranging from 0 to magnitude.
Arr3f* CaveGenerator::GenerateNoise(int sx, int sy, int sz, float magnitude) {
	Arr3f *noiseArray = new Arr3f(sx, sy, sz);
	for (int x = 0; x < sx; x++)
		for (int y = 0; y < sy; y++)
			for (int z = 0; z < sz; z++)
				(*noiseArray)[x][y][z] = FastRandF() * magnitude;
	return noiseArray;
}

// Generates some perlin noise!
void CaveGenerator::GeneratePerlinNoise(Arr3f &data) {
	Arr3f *noise = nullptr, *noise2 = nullptr;
	for (int f = 4; f < 32; f *= 2) {
		noise = GenerateNoise(f, f, f, 2.f / f);
		noise2 = InterpolateData(*noise, data.getX()/f);
		delete noise;
		AddDataTo(data, *noise2);
		delete noise2;
	}
}

// Does a random walk of noiseData, setting cells to 0 in caveData in the process.
void CaveGenerator::PaintWithRandomWalk(Superchunk &sc, Arr3f &noiseData, int paintRadius, BlockType paintValue, bool dontStopAtEdge) {
	const int xs = noiseData.getX(), ys = noiseData.getY(), zs = noiseData.getZ();
	int x = FastRand(0, xs-1);
	int y = FastRand(0, ys-1);
	int z = FastRand(0, zs-1);
	int m = cbrt(xs*ys*zs);

	if (y < ys/50)
		y = 0;

	int count = 0;

	while (count < m) {
		float oldNoise = noiseData[x][y][z];

		PaintAtPoint(sc, x, y, z, paintRadius, paintValue);
		int dx = FastRand(-paintRadius, paintRadius);
		int dy = FastRand(-paintRadius, paintRadius);
		int dz = FastRand(-paintRadius, paintRadius);

		x += dx;
		y += dy;
		z += dz;

		if (x < 0 || y < 0 || z < 0 || x >= xs || y >= ys || z >= zs) {
			if (dontStopAtEdge) {
				++count;
				if (x < 0) x = 0;
				if (y < 0) y = 0;
				if (z < 0) z = 0;
				if (x >= xs) x = xs - 1;
				if (y >= ys) y = ys - 1;
				if (z >= zs) z = zs - 1;
			} else
				break;
		}

		if (y < 0)
			y = 0;

		float newNoise = noiseData[x][y][z];

		// If we're jumping to a higher value on the noise gradient, move twice as far.
		if (newNoise > oldNoise) {
			PaintAtPoint(sc, x, y, z, paintRadius, paintValue);
			x += dx;
			y += dy;
			z += dz;

			if (x < 0 || y < 0 || z < 0 || x >= xs || y >= ys || z >= zs) {
				if (dontStopAtEdge) {
					++count;
					if (x < 0) x = 0;
					if (y < 0) y = 0;
					if (z < 0) z = 0;
					if (x >= xs) x = xs - 1;
					if (y >= ys) y = ys - 1;
					if (z >= zs) z = zs - 1;
				} else
					break;
			}

			if (y < 0)
				y = 0;
		}  
	}
}

void CaveGenerator::PaintWithRandomWalk(Arr3BT &data, Arr3f &noiseData, int paintRadius, BlockType paintValue, bool dontStopAtEdge) {
	const int xs = data.getX(), ys = data.getY(), zs = data.getZ();
	int x = FastRand(0, xs-1);
	int y = FastRand(0, ys-1);
	int z = FastRand(0, zs-1);
	int m = cbrt(xs*ys*zs);

	if (y < ys/50)
		y = 0;

	int count = 0;

	while (count < m) {
		float oldNoise = noiseData[x][y][z];

		PaintAtPoint(data, x, y, z, paintRadius, paintValue);
		int dx = FastRand(-paintRadius, paintRadius);
		int dy = FastRand(-paintRadius, paintRadius);
		int dz = FastRand(-paintRadius, paintRadius);

		x += dx;
		y += dy;
		z += dz;

		if (x < 0 || y < 0 || z < 0 || x >= xs || y >= ys || z >= zs) {
			if (dontStopAtEdge) {
				++count;
				if (x < 0) x = 0;
				if (y < 0) y = 0;
				if (z < 0) z = 0;
				if (x >= xs) x = xs - 1;
				if (y >= ys) y = ys - 1;
				if (z >= zs) z = zs - 1;
			} else
				break;
		}

		if (y < 0)
			y = 0;

		float newNoise = noiseData[x][y][z];

		// If we're jumping to a higher value on the noise gradient, move twice as far.
		if (newNoise > oldNoise) {
			PaintAtPoint(data, x, y, z, paintRadius, paintValue);
			x += dx;
			y += dy;
			z += dz;

			if (x < 0 || y < 0 || z < 0 || x >= xs || y >= ys || z >= zs) {
				if (dontStopAtEdge) {
					count += 1;
					if (x < 0) x = 0;
					if (y < 0) y = 0;
					if (z < 0) z = 0;
					if (x >= xs) x = xs - 1;
					if (y >= ys) y = ys - 1;
					if (z >= zs) z = zs - 1;
				} else
					break;
			}

			if (y < 0)
				y = 0;
		}  
	}
}

void CaveGenerator::PaintAtPoint(Superchunk &sc, int x, int y, int z, int paintRadius, BlockType paintValue) {
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

void CaveGenerator::PaintAtPoint(Arr3BT &data, int x, int y, int z, int paintRadius, BlockType paintValue) {
	int prsq = paintRadius*paintRadius;
	for (int dx = -paintRadius; dx <= paintRadius; dx++) {
		int dxsq = dx*dx;
		for (int dy = -paintRadius; dy <= paintRadius; dy++) {
			int dxdysq = dxsq+dy*dy;
			for (int dz = -paintRadius; dz <= paintRadius; dz++)
				if (x+dx >= 0 && y+dy>= 0 && z+dz >= 0 && x+dx < data.getX() && y+dy < data.getY() && z+dz < data.getZ())
					if (dxdysq+dz*dz<prsq)
						data[x + dx][y + dy][z + dz] = paintValue;
		}
	}
}

Arr3f* CaveGenerator::GenerateGradient(int x, int y, int z) {
	Arr3f *data = new Arr3f(x, y, z);

	for (int j = 0; j < y; j++) {
		float v = (float)j/y;
		for (int i = 0; i < x; i++)
			for (int k = 0; k < z; k++)
				(*data)[i][j][k] = v;
	}

	return data;
}

template<typename T> T clamp(T v, T min, T max) {
	if (v > max)
		return max;
	if (v < min)
		return min;
	return v;
}

// Radial gradient concentrated with high values at the outside.
Arr3f* CaveGenerator::GenerateRadialGradient(int x, int y, int z) {
	Arr3f *data = new Arr3f(x, y, z);
	const int size = cbrt(x*y*z);

	for (int i = 0; i < x; i++)
		for (int j = 0; j < y; j++)
			for (int k = 0; k < z; k++) {
				float dist = (float)sqrt(pow(x - size / 2, 2) + pow(y - size / 2, 2));
				(*data)[i][j][k] = clamp(dist / size * 0.3f * (float)z / size, 0.f, 1.f);
			}
	return data;
}

// Adds the values in dataSrc to the values in dataDst, storing the result in dataDst.
void CaveGenerator::AddDataTo(Arr3f &dataDst, Arr3f &dataSrc, float scalarDst, float scalarSrc) {
	assert(dataDst.sizeEqual(dataSrc));
	
	for (int x = 0; x < dataDst.getX(); x++)
		for (int y = 0; y < dataDst.getY(); y++)
			for (int z = 0; z < dataDst.getZ(); z++)
				dataDst[x][y][z] = clamp(dataDst[x][y][z]*scalarDst + dataSrc[x][y][z]*scalarSrc, 1.f, 0.f);
}

void CaveGenerator::AddDataTo(Arr3f &dataDst, Arr3f &dataSrc) {
	AddDataTo(dataDst, dataSrc, 1, 1);
}

// Resizes dataIn, with size sizeIn, to be of size sizeOut.
Arr3f* CaveGenerator::InterpolateData(const Arr3f &dataIn, int scale) {
	int xb = dataIn.getX(), yb = dataIn.getY(), zb = dataIn.getZ();
	int xs = xb*scale, ys = yb*scale, zs = zb*scale;
	Arr3f *dataOut = new Arr3f(xs, ys, zs);
	int r = scale;
	for (int x = 0; x < xs; x++)
		for (int y = 0; y < ys; y++)
			for (int z = 0; z < zs; z++) {
				int xIn0 = x / r,       yIn0 = y / r,       zIn0 = z / r;
				int xIn1 = xIn0 + 1,    yIn1 = yIn0 + 1,    zIn1 = zIn0 + 1;
				if (xIn1 >= xb)
					xIn1 = 0;
				if (yIn1 >= yb)
					yIn1 = 0;
				if (zIn1 >= zb)
					zIn1 = 0;

				float v000 = dataIn[xIn0][yIn0][zIn0];
				float v100 = dataIn[xIn1][yIn0][zIn0];
				float v010 = dataIn[xIn0][yIn1][zIn0];
				float v110 = dataIn[xIn1][yIn1][zIn0];
				float v001 = dataIn[xIn0][yIn0][zIn1];
				float v101 = dataIn[xIn1][yIn0][zIn1];
				float v011 = dataIn[xIn0][yIn1][zIn1];
				float v111 = dataIn[xIn1][yIn1][zIn1];

				float xS = ((float)(x % r)) / r;
				float yS = ((float)(y % r)) / r;
				float zS = ((float)(z % r)) / r;

				(*dataOut)[x][y][z] =  v000 * (1 - xS) * (1 - yS) * (1 - zS) +
									v100 * xS * (1 - yS) * (1 - zS) +
									v010 * (1 - xS) * yS * (1 - zS) +
									v001 * (1 - xS) * (1 - yS) * zS +
									v101 * xS * (1 - yS) * zS +
									v011 * (1 - xS) * yS * zS +
									v110 * xS * yS * (1 - zS) +
									v111 * xS * yS * zS;
			}
	return dataOut;
}

void CaveGenerator::RenderSlice(const Arr3BT &data, int z, Texture &renderTexture) {
	int w = data.getX(), h = data.getY();
	uint8 *pixelData = new uint8[w * h * 4];
	for (int x = 0; x < 256; x++) {
		for (int y = 0; y < 256; y++) {
			uint c = 0xFF000000;
			if (data[x][y][z] == BlockType::Dirt)
				c = 0xFFFFFFFF;
			if (data[x][y][z] == BlockType::Ore)
				c = 0xFF888888;
			if (data[x][y][z] == BlockType::Gold)
				c = 0xFFFF0000;
			if (data[x][y][z] == BlockType::Rock)
				c = 0xFF0000FF;
			pixelData[y * w + x] = c;
		}
	}
	renderTexture.setTexture(w, h, pixelData, Texture::PixelFormat::RGBA);
	delete[] pixelData;
}

}
