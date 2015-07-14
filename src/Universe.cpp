#include "Universe.hpp"

namespace Diggler {

Universe::Universe(Game *G) : G(G) {
}

Universe::~Universe() {
}

WorldRef Universe::getWorld(WorldId id) {
	iterator it = find(id);
	if (it == end())
		return WorldRef();
	return it->second.lock();
}

WorldRef Universe::getWorldEx(WorldId id) {
	// TODO World loading
	iterator it = find(id);
	if (it == end()) {
		WorldRef w = std::make_shared<World>(G, id);
		emplace(std::piecewise_construct, std::tuple<WorldId>(id), std::tuple<WorldRef>(w));
		return w;
	}
	return it->second.lock();
}

WorldRef Universe::createWorld(WorldId id) {
	WorldRef w = std::make_shared<World>(G, id);
	emplace(std::piecewise_construct, std::tuple<WorldId>(id), std::tuple<WorldRef>(w));
	return w;
}

void Universe::recv(Net::InMessage &msg) {

}

}