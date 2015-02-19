#ifndef AABOX_HPP
#define AABOX_HPP
#include <glm/glm.hpp>

namespace Diggler {

class AABB {
public:
	glm::vec3 v1, v2;
	AABB(const glm::vec3 &vec1 = glm::vec3(), const glm::vec3 &vec2 = glm::vec3());
	void set(const glm::vec3 &vec1, const glm::vec3 &vec2);
	bool intersects(const glm::vec3 &point) const;
	bool intersects(const AABB &other) const;
};

}

#endif