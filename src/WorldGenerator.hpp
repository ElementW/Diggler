#ifndef DIGGLER_WORLD_GENERATOR_HPP
#define DIGGLER_WORLD_GENERATOR_HPP

#include "platform/types/vec3.hpp"

namespace diggler {

class World;

class WorldGenerator {
public:
  virtual void generate(World &world, vec3i chunkPos);
};

}

#endif /* DIGGLER_WORLD_GENERATOR_HPP */
