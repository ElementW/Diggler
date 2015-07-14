#ifndef ENTITY_HPP
#define ENTITY_HPP
#include "Platform.hpp"

namespace Diggler {

class Entity {
public:
	enum class PropType {
		Int8,
		Int32,
		Int64,
		UInt8,
		UInt32,
		UInt64,
		Float,
		Double,
		String,
		Vec2,
		Vec3,
		Color
	};
protected:
	AddProp(void*, PropType, const String &name, const String &desc = "");
};

}

#endif