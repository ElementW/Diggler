#include "Frustum.hpp"
#include <cmath>
#include <glm/glm.hpp>

namespace Diggler {

void Frustum::setCamInternals(vec3ct rad, vec3ct ratio, vec3ct nearD, vec3ct farD) {
  this->ratio = ratio;
  this->angle = rad;
  this->nearD = nearD;
  this->farD = farD;

  // Compute ALL the things
  tang = std::tan(rad * static_cast<vec3ct>(.5));
  nh = nearD * tang;
  nw = nh * ratio;
  fh = farD  * tang;
  fw = fh * ratio;
}

void Frustum::setCamDef(const vec3 &p, const vec3 &l, const vec3 &u) {
  vec3 dir,nc,fc,X,Y, Z = glm::normalize(p - l);

  // X axis of camera with given "up" vector and Z axis
  X = glm::normalize(glm::cross(u, Z));

  // the real "up" vector is the cross product of Z and X
  Y = glm::cross(Z, X);

  // compute the centers of the near and far planes
  nc = p - Z * nearD;
  fc = p - Z * farD;

  // compute the 4 corners of the frustum on the near plane
  ntl = nc + Y * nh - X * nw;
  ntr = nc + Y * nh + X * nw;
  nbl = nc - Y * nh - X * nw;
  nbr = nc - Y * nh + X * nw;

  // compute the 4 corners of the frustum on the far plane
  ftl = fc + Y * fh - X * fw;
  ftr = fc + Y * fh + X * fw;
  fbl = fc - Y * fh - X * fw;
  fbr = fc - Y * fh + X * fw;

  pl[TOP].set3Points(ntr,ntl,ftl);
  pl[BOTTOM].set3Points(nbl,nbr,fbr);
  pl[LEFT].set3Points(ntl,nbl,fbl);
  pl[RIGHT].set3Points(nbr,ntr,fbr);
  pl[NEAR].set3Points(ntl,ntr,nbr);
  pl[FAR].set3Points(ftr,ftl,fbl);
}

bool Frustum::pointInFrustum(const vec3& p) const {
  for(int i=0; i < 6; i++) {
    if (pl[i].distance(p) < static_cast<vec3ct>(0))
      return false;
  }
  return true;
}

bool Frustum::sphereInFrustum(const vec3 &p, vec3ct radius) const {
  for(int i=0; i < 6; i++) {
    if (pl[i].distance(p) < -radius)
      return false;
  }
  return true;
}

}
