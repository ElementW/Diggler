#ifndef CAVE_GENERATOR_HPP
#define CAVE_GENERATOR_HPP
#include <string>
#include "World.hpp"
#include "WorldGenerator.hpp"

namespace Diggler {

class CaveGenerator : public WorldGenerator {
private:
  CaveGenerator();

public:
  struct GenConf {
    uint seed;
    double groundLevel;
    struct {
      bool enabled;
      double length;
      int count;
    } cave;
    struct {
      bool enabled;
      double thresold;
    } ore;
    struct {
      bool enabled;
      double freq, freqAmpl, minLevel, maxLevel;
    } lava, diamond;
    double oreFactor;
    struct {
      bool enabled;
      double freq, freqAmpl, minLevel, maxLevel;
      uint minSize, maxSize;
    } rocks, gold;
    GenConf();
  };

  static void Generate(WorldRef, const GenConf&, ChunkRef);
};

}

#endif