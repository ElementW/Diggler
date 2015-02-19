#ifndef CHUNK_CHANGE_HELPER_HPP
#define CHUNK_CHANGE_HELPER_HPP
#include "Blocks.hpp"
#include "network/Network.hpp"

namespace Diggler {

class ChunkChangeHelper {
private:
	struct Change {
		int x, y, z;
		BlockType b;
	};
	std::vector<Change> m_changes;

public:
	ChunkChangeHelper();
	void add(int x, int y, int z, BlockType b);
	bool empty() const;
	int count() const;
	void flush(Net::OutMessage&);
	void discard();
};

}

#endif