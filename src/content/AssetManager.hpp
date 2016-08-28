#ifndef DIGGLER_CONTENT_ASSET_MANAGER_HPP
#define DIGGLER_CONTENT_ASSET_MANAGER_HPP

#include "Asset.hpp"
#include "Mod.hpp"

namespace Diggler {

class Game;

namespace Content {

class AssetManager final {
private:
  Game *G;

public:
  AssetManager(Game*);


};

}
}

#endif /* DIGGLER_CONTENT_ASSET_MANAGER_HPP */
