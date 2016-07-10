#ifndef FRUSTUM_HPP
#define FRUSTUM_HPP

#include <glm/glm.hpp>

namespace Diggler {

class Frustum {
private:
  enum {
    TOP = 0, BOTTOM, LEFT,
    RIGHT, NEAR, FAR
  };

public:
  class Plane {
  public:
    glm::vec3 normal;
    float d;

    Plane(const glm::vec3 &v1, const glm::vec3 &v2, const glm::vec3 &v3) {
      set3Points(v1, v2, v3);
    }
    Plane() {}
    ~Plane() {}

    void set3Points(const glm::vec3 &v1, const glm::vec3 &v2, const glm::vec3 &v3) {
      const glm::vec3 aux1 = v1 - v2, aux2 = v3 - v2;
      normal = glm::normalize(glm::cross(aux2, aux1));
      d = -glm::dot(normal, v2);
    }
    void setNormalAndPoint(const glm::vec3 &normal, const glm::vec3 &point) {
      this->normal = glm::normalize(normal);
      d = -glm::dot(normal, point);
    }
    void setCoefficients(float a, float b, float c, float d) {
      normal = glm::vec3(a, b, c); 
      float l = glm::length(normal);
      normal /= l; // normalizes
      this->d = d/l;
    }
    float distance(const glm::vec3 &p) const {
      return (d + glm::dot(normal, p));
    }
  } pl[6];
  glm::vec3 ntl,ntr,nbl,nbr,ftl,ftr,fbl,fbr;
  float nearD, farD, ratio, angle,tang;
  float nw,nh,fw,fh;

  Frustum() = default;
  ~Frustum() = default;

  void setCamInternals(float rad, float ratio, float nearD, float farD);
  void setCamDef(const glm::vec3 &p, const glm::vec3 &l, const glm::vec3 &u);
  bool pointInFrustum(const glm::vec3 &p) const;
  bool sphereInFrustum(const glm::vec3 &p, float radius) const;
  //bool boxInFrustum(const AABB &b) const;
};

}

#endif
