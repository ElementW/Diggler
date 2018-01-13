#ifndef DIGGLER_CONTENT_ASSET_MANAGER_HPP
#define DIGGLER_CONTENT_ASSET_MANAGER_HPP

#include <map>
#include <memory>

#include "Asset.hpp"
#include "Mod.hpp"

namespace diggler {

class Game;

namespace content {

class AssetManager final {
private:
  Game *G;
  std::map<Crypto::SHA256::Digest, std::unique_ptr<Asset>> m_cachedAssets;
  std::map<Crypto::SHA256::Digest, std::unique_ptr<AssetContentMetadata>> m_cachedContentMetadata;

public:
  AssetManager(Game*);


};

}
}

#endif /* DIGGLER_CONTENT_ASSET_MANAGER_HPP */
