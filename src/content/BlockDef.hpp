#ifndef DIGGLER_BLOCK_DEF_HPP
#define DIGGLER_BLOCK_DEF_HPP

#include <unordered_map>

#include "ObjectDef.hpp"
#include "../Texture.hpp"
#include "../util/TexturePacker.hpp"
//#include "../AABB.hpp"

namespace Diggler {

class BlockDef : public ObjectDef {
public:
  struct Appearance {
    Variability variability;
    struct Texture {
      Diggler::Texture *tex;
      TexturePacker::Coord coord;
      std::vector<TexturePacker::Coord> divCoords;
      struct Repeat {
        uint8 xdiv, ydiv;
      } repeat;
    };
    std::unordered_map<std::string, Texture> textures;
    struct Look {
      enum class Type : uint8 {
        Hidden = 0,
        Cube = 1
      } type;
      union Data {
        struct Cube {
          struct {
            std::unordered_map<std::string, Texture>::iterator texture;
          } sides[6];
        } cube;

        Data() {}
      } data;
    } look;
  } appearance;

  struct PhysicalProperties {
    bool hasCollision;
    bool fullBlock;

    PhysicalProperties() :
      hasCollision(true),
      fullBlock(true) {
    }
  } phys;
};

}

#endif /* DIGGLER_BLOCK_DEF_HPP */
