#include "Camera.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>

namespace diggler {

Camera::Camera() {
  m_worldUp = vec3(0, 1, 0);
  update();
}

void Camera::setProjection(const mat4& m) {
  m_projMatrix = m;
  update();
}

void Camera::setPersp(float rad, float ratio, float near, float far) {
  frustum.setCamInternals(rad, ratio, near, far);
  setProjection(glm::perspective(rad, ratio, near, far));
}

void Camera::lookAt(const vec3& l) {
  m_lookAt = l;
  update();
}

void Camera::lookAtAbs(const vec3& l) {
  m_lookAt = m_position - l;
  update();
}

void Camera::setPosition(const vec3& p) {
  m_position = p;
  update();
}

void Camera::update() {
  m_viewMatrix = glm::lookAt(m_position, m_position + m_lookAt, m_worldUp);
  frustum.setCamDef(m_position, m_position + m_lookAt, m_worldUp);
  m_up = glm::normalize(glm::cross(glm::cross(m_lookAt, m_worldUp), m_lookAt));
  m_pvMatrix = m_projMatrix * m_viewMatrix;
  m_skyMatrix = m_projMatrix * glm::lookAt(vec3(), m_lookAt, m_worldUp);
}

const mat4& Camera::getVMatrix() const {
  return m_viewMatrix;
}

const mat4& Camera::getPVMatrix() const {
  return m_pvMatrix;
}

const mat4& Camera::getSkyMatrix() const {
  return m_skyMatrix;
}

const vec3& Camera::getUp() const {
  return m_up;
}

const vec3& Camera::getLookAt() const {
  return m_lookAt;
}

}
