#ifndef DIGGLER_CAMERA_HPP
#define DIGGLER_CAMERA_HPP

#include "platform/types/mat4.hpp"
#include "platform/types/vec3.hpp"

#include "Frustum.hpp"

namespace diggler {

class Camera {
  friend class LocalPlayer;
private:
  mat4 m_projMatrix, m_viewMatrix, m_pvMatrix, m_skyMatrix;
  vec3 m_position, m_lookAt, m_worldUp, m_up;
  
  void update();
  void setProjection(const mat4& m);

public:
  Frustum frustum;
  
  Camera();
  void setPersp(float rad, float ratio, float near, float far);
  void setPosition(const vec3 &p);
  void lookAtAbs(const vec3 &l);
  void lookAt(const vec3 &l);
  const mat4& getVMatrix() const;
  const mat4& getPVMatrix() const;
  const mat4& getSkyMatrix() const;
  const vec3& getUp() const;
  const vec3& getLookAt() const;
};

}

#endif
