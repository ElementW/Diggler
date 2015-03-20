#include "ChunkChangeHelper.hpp"

namespace Diggler {

ChunkChangeHelper::ChunkChangeHelper() : enabled(true) {
	m_changes.reserve(64);
}

void ChunkChangeHelper::add(int x, int y, int z, BlockType b) {
	if (!enabled)
		return;
	for (Change &c : m_changes) {
		if (c.x == x && c.y == y && c.z == z) {
			c.b = b;
			return;
		}
	}
	m_changes.emplace_back(Change {x, y, z, b});
}

void ChunkChangeHelper::flush(Net::OutMessage &msg) {
	for (Change &c : m_changes) {
		msg.writeU16(c.x);
		msg.writeU16(c.y);
		msg.writeU16(c.z);
		msg.writeU8(static_cast<uint8>(c.b));
	}
	m_changes.clear();
}

int ChunkChangeHelper::count() const {
	return m_changes.size();
}

bool ChunkChangeHelper::empty() const {
	return m_changes.empty();
}

void ChunkChangeHelper::discard(){
	m_changes.clear();
}

}