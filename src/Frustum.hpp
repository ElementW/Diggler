#ifndef DIGGLER_FRUSTUM_HPP
#define DIGGLER_FRUSTUM_HPP

#include <glm/geometric.hpp>

#include "platform/types/vec3.hpp"

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
    vec3 normal;
    vec3ct d;

    Plane(const vec3 &v1, const vec3 &v2, const vec3 &v3) {
      set3Points(v1, v2, v3);
    }
    Plane() {}
    ~Plane() {}

    void set3Points(const vec3 &v1, const vec3 &v2, const vec3 &v3) {
      const vec3 aux1 = v1 - v2, aux2 = v3 - v2;
      normal = glm::normalize(glm::cross(aux2, aux1));
      d = -glm::dot(normal, v2);
    }
    void setNormalAndPoint(const vec3 &normal, const vec3 &point) {
      this->normal = glm::normalize(normal);
      d = -glm::dot(normal, point);
    }
    void setCoefficients(vec3ct a, vec3ct b, vec3ct c, vec3ct d) {
      normal = vec3(a, b, c);
      vec3ct l = glm::length(normal);
      normal /= l; // normalizes
      this->d = d / l;
    }
    vec3ct distance(const vec3 &p) const {
      return (d + glm::dot(normal, p));
    }
  } pl[6];
  vec3 ntl,ntr,nbl,nbr,ftl,ftr,fbl,fbr;
  vec3ct nearD, farD, ratio, angle,tang;
  vec3ct nw,nh,fw,fh;

  Frustum() = default;
  ~Frustum() = default;

  void setCamInternals(vec3ct rad, vec3ct ratio, vec3ct nearD, vec3ct farD);
  void setCamDef(const vec3 &p, const vec3 &l, const vec3 &u);
  bool pointInFrustum(const vec3 &p) const;
  bool sphereInFrustum(const vec3 &p, vec3ct radius) const;
  //bool boxInFrustum(const AABB &b) const;
};

}

#endif /* DIGGLER_FRUSTUM_HPP */
