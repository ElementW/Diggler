#ifndef CAMERA_HPP
#define CAMERA_HPP
#include <glm/glm.hpp>
#include "Frustum.hpp"

namespace Diggler {

class Camera {
	friend class LocalPlayer;
private:
	glm::mat4 m_projMatrix, m_viewMatrix, m_pvMatrix, m_skyMatrix;
	glm::vec3 m_position, m_lookAt, m_worldUp, m_up;
	
	void update();
	void setProjection(const glm::mat4& m);

public:
	Frustum frustum;
	
	Camera();
	void setPersp(float rad, float ratio, float near, float far);
	void setPosition(const glm::vec3 &p);
	void lookAtAbs(const glm::vec3 &l);
	void lookAt(const glm::vec3 &l);
	const glm::mat4& getVMatrix() const;
	const glm::mat4& getPVMatrix() const;
	const glm::mat4& getSkyMatrix() const;
	const glm::vec3& getUp() const;
	const glm::vec3& getLookAt() const;
};

}

#endif