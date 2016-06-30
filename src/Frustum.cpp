#include "Frustum.hpp"
#include <cmath>
#include <glm/glm.hpp>

namespace Diggler {

Frustum::Frustum() {

}

Frustum::~Frustum() {

}

void Frustum::setCamInternals(float rad, float ratio, float nearD, float farD) {
  this->ratio = ratio;
  this->angle = rad;
  this->nearD = nearD;
  this->farD = farD;

  // Compute ALL the things
  tang = (float)std::tan(rad * 0.5f);
  nh = nearD * tang;
  nw = nh * ratio;
  fh = farD  * tang;
  fw = fh * ratio;
}

void Frustum::setCamDef(const glm::vec3 &p, const glm::vec3 &l, const glm::vec3 &u) {
  glm::vec3 dir,nc,fc,X,Y, Z = glm::normalize(p - l);

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

bool Frustum::pointInFrustum(const glm::vec3& p) const {
  for(int i=0; i < 6; i++) {
    if (pl[i].distance(p) < 0.f)
      return false;
  }
  return true;
}

bool Frustum::sphereInFrustum(const glm::vec3 &p, float radius) const {
  for(int i=0; i < 6; i++) {
    if (pl[i].distance(p) < -radius)
      return false;
  }
  return true;
}

}