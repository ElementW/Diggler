#ifndef BLOCK_DEF_HPP
#define BLOCK_DEF_HPP
#include "ObjectDef.hpp"
#include "../AABB.hpp"

namespace Diggler {

class BlockDef : public ObjectDef {
public:
	bool solid, fullBlock;
	AABBVector boxes;
	
};

}

#endif