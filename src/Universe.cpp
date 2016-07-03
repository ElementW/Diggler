#include "Universe.hpp"

namespace Diggler {

Universe::Universe(Game *G, bool remote) :
  G(G), isRemote(remote) {
}

Universe::~Universe() {
}

WorldRef Universe::getWorld(WorldId id) {
  iterator it = find(id);
  if (it == end())
    return WorldRef();
  return it->second.lock();
}

WorldRef Universe::getLoadWorld(WorldId id) {
  // TODO World loading
  iterator it = find(id);
  if (it == end()) {
    WorldRef w = std::make_shared<World>(G, id, isRemote);
    emplace(std::piecewise_construct, std::forward_as_tuple(id), std::forward_as_tuple(w));
    return w;
  }
  return it->second.lock();
}

WorldRef Universe::createWorld(WorldId id) {
  WorldRef w = std::make_shared<World>(G, id, isRemote);
  emplace(std::piecewise_construct, std::forward_as_tuple(id), std::forward_as_tuple(w));
  return w;
}

void Universe::recv(Net::InMessage &msg) {

}

}
