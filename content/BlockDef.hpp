#ifndef BLOCK_DEF_HPP
#define BLOCK_DEF_HPP
#include "ObjectDef.hpp"

namespace Diggler {
namespace Content {

class BlockDef : public ObjectDef {
public:
	bool fullBlock;
	struct Boxes {
		int count;
		
	} *boxes;
};

}
}

#endif