#ifndef OBJECT_DEF_HPP
#define OBJECT_DEF_HPP
#include "../Platform.hpp"

namespace Diggler {
namespace Content {

class ObjectDef {
public:
	/**
	 * Energy-Matter Value
	 * Stone has an EMV of 10
	 */
	int emv;

	
	enum class Variability : uint8 {
		Static,
		Dynamic,
		Stream
	} visualVariability;
};

}
}

#endif