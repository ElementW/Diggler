#include "Registry.h"

#include "BlockDef.h"
#include "../../../../content/Registry.hpp"
#include "../../../../Game.hpp"

using namespace Diggler;

void Diggler_Content_Registry_registerBlock(struct Diggler_Game *cG,
  const char *name, struct Diggler_Content_BlockDef *cBdef) {
  using namespace Content;
  Game &G = *reinterpret_cast<Game*>(cG);
  Registry::BlockRegistration br(G.CR->registerBlock(name));
  { decltype(cBdef->appearance) &cApp = cBdef->appearance;
    decltype(br.def.appearance) &app = br.def.appearance;
    std::vector<decltype(app.textures)::iterator> textureIts;
    textureIts.reserve(cApp.texturesCount);
    for (int i = 0; i < cApp.texturesCount; ++i) {
      decltype(*cApp.textures) &cTex = cApp.textures[i];
      std::pair<decltype(app.textures)::iterator, bool> itPair =
        app.textures.emplace(std::piecewise_construct,
        std::forward_as_tuple(cTex.name),
        std::forward_as_tuple());
      decltype(app.textures)::iterator &it = itPair.first;
      decltype(app.textures)::value_type::second_type &tex = it->second;
      tex.coord = G.CR->addTexture(cTex.name, cTex.path);
      cTex.repeatXdiv = cTex.repeatYdiv = 4;
      tex.repeat.xdiv = cTex.repeatXdiv;
      tex.repeat.ydiv = cTex.repeatYdiv;
      if (cTex.repeatXdiv > 1 || cTex.repeatYdiv > 1) {
        uint16 width  = (tex.coord.u - tex.coord.x) / cTex.repeatXdiv,
               height = (tex.coord.v - tex.coord.y) / cTex.repeatYdiv;
        for (int16 y = cTex.repeatYdiv - 1; y >= 0; --y) {
          for (int16 x = cTex.repeatXdiv - 1; x >= 0; --x) {
            tex.divCoords.emplace_back(Util::TexturePacker::Coord {
              static_cast<uint16>(tex.coord.x + width * x),
              static_cast<uint16>(tex.coord.y + height * y),
              static_cast<uint16>(tex.coord.x + width * (x + 1)),
              static_cast<uint16>(tex.coord.y + height * (y + 1))
            });
          }
        }
      }
      textureIts.emplace_back(it);
    }
    { decltype(cApp.look) &cLook = cApp.look;
      decltype(app.look) &look = app.look;
      using Type = BlockDef::Appearance::Look::Type;
      look.type = static_cast<Type>(cLook.type);
      switch (look.type) {
      case Type::Cube:
        for (int i = 0; i < 6 /* Math::Geometry::Cube::FaceCount */; ++i) {
          look.data.cube.sides[i].texture = textureIts.at(cLook.data.cube.sides[i].texture);
        }
        break;
      case Type::Hidden:
        break;
      }
    }
  }
  br.commit();
}
