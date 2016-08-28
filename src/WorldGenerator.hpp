#ifndef DIGGLER_WORLD_GENERATOR_HPP
#define DIGGLER_WORLD_GENERATOR_HPP

#include <glm/vec3.hpp>

namespace Diggler {

class World;

class WorldGenerator {
public:
  virtual void generate(World &W, glm::ivec3 CP);
};

}

#endif /* DIGGLER_WORLD_GENERATOR_HPP */
