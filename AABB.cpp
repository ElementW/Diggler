#include "AABB.hpp"
#include <memory>

namespace Diggler {

AABB::AABB(const glm::vec3 &vec1, const glm::vec3 &vec2) {
	set(vec1, vec2);
}

void AABB::set(const glm::vec3 &vec1, const glm::vec3 &vec2) {
	v1 = vec1;
	v2 = vec2;
	if (v1.x > v2.x)
		std::swap(v1.x, v2.x);
	if (v1.y > v2.y)
		std::swap(v1.y, v2.y);
	if (v1.z > v2.z)
		std::swap(v1.z, v2.z);
}

bool AABB::intersects(const glm::vec3 &point) const {
	if (point.x > v1.x && point.x < v2.x &&
		point.y > v1.y && point.y < v2.y &&
		point.z > v1.z && point.z < v2.z)
		return true;
	return false;
}

bool AABB::intersects(const AABB &other) const {
	bool xOverlap = !(other.v1.x > v2.x || other.v2.x < v1.x);
	bool yOverlap = !(other.v1.y > v2.y || other.v2.y < v1.y);
	bool zOverlap = !(other.v1.z > v2.z || other.v2.z < v1.z);
	return xOverlap && yOverlap && zOverlap;
}

}