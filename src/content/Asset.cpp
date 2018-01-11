#include "Asset.hpp"

#include <meiose/variant.hpp>

#include "AssetManager.hpp"

namespace Diggler {
namespace Content {

Asset::Asset(const std::shared_ptr<AssetContentMetadata> &acm) :
  m_contentMetadata(acm) {
}



}
}
