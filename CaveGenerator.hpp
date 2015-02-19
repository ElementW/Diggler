#ifndef CAVE_GENERATOR_HPP
#define CAVE_GENERATOR_HPP
#include <string>
#include "Superchunk.hpp"
#include "Arr3Utils.hpp"

namespace Diggler {

typedef Arr3<BlockType> Arr3BT;

class CaveGenerator {
private:
	CaveGenerator();
	
public:
	static std::string CaveInfo;
	static int GROUND_LEVEL;
	static void GenerateConstant(Superchunk &sc, BlockType value);
	static void GenerateConstant(Arr3BT &data, BlockType value);
	static void GenerateCaveSystem(Superchunk& sc, bool includeLava, uint oreFactor);
	static Arr3f* GenerateNoise(int sx, int sy, int sz, float magnitude);
	static void GeneratePerlinNoise(Arr3f &data);
	static Arr3f* InterpolateData(const Arr3f &dataIn, int scale);
	static Arr3f* GenerateRadialGradient(int x, int y, int z);
	static Arr3f* GenerateGradient(int x, int y, int z);
	static void PaintWithRandomWalk(Superchunk &sc, Arr3f &noiseData, int paintRadius, BlockType paintValue, bool dontStopAtEdge);
	static void PaintWithRandomWalk(Arr3BT &data, Arr3f &noiseData, int paintRadius, BlockType paintValue, bool dontStopAtEdge);
	static void PaintAtPoint(Superchunk &sc, int x, int y, int z, int paintRadius, BlockType paintValue);
	static void PaintAtPoint(Arr3BT &data, int x, int y, int z, int paintRadius, BlockType paintValue);
	static void AddDataTo(Arr3f &dataDst, Arr3f &dataSrc);
	static void AddDataTo(Arr3f &dataDst, Arr3f &dataSrc, float scalarDst, float scalarSrc);
	static void AddDiamond(Superchunk &sc);
	static void AddGold(Superchunk &sc);
	static void AddRocks(Superchunk &sc);
	static void AddLava(Superchunk &sc);
	/// Renders a specific z-level of a 256x256x256 data array to a texture.
	void RenderSlice(const Arr3BT &data, int z, Texture &renderTexture);
};

}

#endif