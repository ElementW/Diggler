#include "Asset.hpp"

#include <meiose/variant.hpp>

#include "AssetManager.hpp"

namespace diggler {
namespace content {

Asset::Asset(const std::shared_ptr<AssetContentMetadata> &acm) :
  m_contentMetadata(acm) {
}



}
}
