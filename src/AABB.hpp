#ifndef DIGGLER_AABB_HPP
#define DIGGLER_AABB_HPP

#include <limits>
#include <vector>

#include "platform/types/vec3.hpp"

namespace diggler {

///
/// @brief Axis-Aligned Bounding Box.
///
template<typename Vec3 = vec3>
class AABB {
public:
  using T = typename Vec3::value_type;

  union {
    struct { Vec3 v1, v2; };
    struct { Vec3 min, max; };
  };

  AABB(const Vec3 &vec1 = Vec3(), const Vec3 &vec2 = Vec3()) {
    set(vec1, vec2);
  }

  ///
  /// @brief Sets the AABB points.
  /// @param vec1 First point.
  /// @param vec2 Second point.
  /// @note Points coordinates are sorted so that each individual coordinate of vec2 is greater than vec1.
  ///
  void set(const Vec3 &vec1, const Vec3 &vec2) {
    v1 = vec1;
    v2 = vec2;
    if (v1.x > v2.x)
      std::swap(v1.x, v2.x);
    if (v1.y > v2.y)
      std::swap(v1.y, v2.y);
    if (v1.z > v2.z)
      std::swap(v1.z, v2.z);
  }

  ///
  /// @brief Checks if a point is in the AABB.
  /// @param point Point to check intersection with.
  /// @returns `true` if point is in AABB, `false` otherwise.
  ///
  bool intersects(const Vec3 &point) const {
    if (point.x > v1.x && point.x < v2.x &&
      point.y > v1.y && point.y < v2.y &&
      point.z > v1.z && point.z < v2.z)
      return true;
    return false;
  }

  ///
  /// @brief Checks if another AABB intersects this one.
  /// @param other AABB to check intersection with.
  /// @returns `true` if AABB intersects, `false` otherwise.
  ///
  bool intersects(const AABB<Vec3> &other) const {
    bool xOverlap = !(other.v1.x > v2.x || other.v2.x < v1.x);
    bool yOverlap = !(other.v1.y > v2.y || other.v2.y < v1.y);
    bool zOverlap = !(other.v1.z > v2.z || other.v2.z < v1.z);
    return xOverlap && yOverlap && zOverlap;
  }

  ///
  /// @brief Does swept AABB collision computation.
  /// @param other Other AABB to check swept collsion with.
  /// @param vx,vy,vz Velocity.
  /// @param[out] nx,ny,nz Collision normal.
  ///
  float sweptCollision(AABB<Vec3> other, float vx, float vy, float vz, float &nx, float &ny, float &nz) {
    T xInvEntry, yInvEntry, zInvEntry;
    T xInvExit, yInvExit, zInvExit;

    // find the distance between the objects on the near and far sides for both x and y
    if (vx > 0.0f) {
      xInvEntry = other.v1.x - v2.x;
      xInvExit =  other.v2.x - v1.x;
    } else {
      xInvEntry = other.v2.x - v1.x;
      xInvExit =  other.v1.x - v2.x;
    }

    if (vy > 0.0f) {
      yInvEntry = other.v1.y - v2.y;
      yInvExit =  other.v2.y - v1.y;
    } else {
      yInvEntry = other.v2.y - v1.y;
      yInvExit =  other.v1.y - v2.y;
    }

    if (vz > 0.0f) {
      zInvEntry = other.v1.z - v2.z;
      zInvExit =  other.v2.z - v1.z;
    } else {
      zInvEntry = other.v2.z - v1.z;
      zInvExit =  other.v1.z - v2.z;
    }

    // find time of collision and time of leaving for each axis (if statement is to prevent divide by zero)
    float xEntry, yEntry, zEntry;
    float xExit, yExit, zExit;

    if (vx == 0.0f) {
      xEntry = -std::numeric_limits<float>::infinity();
      xExit = std::numeric_limits<float>::infinity();
    } else {
      xEntry = xInvEntry / vx;
      xExit = xInvExit / vx;
    }

    if (vy == 0.0f) {
      yEntry = -std::numeric_limits<float>::infinity();
      yExit = std::numeric_limits<float>::infinity();
    } else {
      yEntry = yInvEntry / vy;
      yExit = yInvExit / vy;
    }

    if (vz == 0.0f) {
      zEntry = -std::numeric_limits<float>::infinity();
      zExit = std::numeric_limits<float>::infinity();
    } else {
      zEntry = zInvEntry / vz;
      zExit = zInvExit / vz;
    }

    // find the earliest/latest times of collision
    float entryTime = std::max(std::max(xEntry, yEntry), zEntry);
    float exitTime = std::min(std::min(xExit, yExit), zExit);

    if ( entryTime > exitTime ||
        (xEntry < 0.0f && yEntry < 0.0f && zEntry < 0.0f) ||
        (xEntry > 1.0f || yEntry > 1.0f || zEntry > 1.0f)) {
      // No collision
      nx = ny = nz = 0.0f;
      return 1.0f;
    } else { // O noes I haz hit!
      if (xEntry > yEntry && yEntry > zEntry) {
        ny = nz = 0.0f;
        nx = xInvEntry < 0.0f ? 1.0f : -1.0f;
      } else if (yEntry > zEntry) {
        nx = nz = 0.0f;
        ny = yInvEntry < 0.0f ? 1.0f : -1.0f;
      } else {
        nx = ny = 0.0f;
        nz = zInvEntry < 0.0f ? 1.0f : -1.0f;
      }

      // return the time of collision
      return entryTime;
    }
  }
};

extern template class AABB<>;

template<typename Vec3 = typename glm::vec3>
class AABBVector : public std::vector<AABB<Vec3>> {
public:
  using T = typename Vec3::value_type;

  ///
  /// @brief Checks if a point is in the AABB vector.
  /// @param point Point to check intersection with.
  /// @returns `true` if point is in one AABB or more, `false` otherwise.
  ///
  bool intersects(const Vec3 &point) const {
    for (const AABB<Vec3> &box : *this)
      if (box.intersects(point))
        return true;
    return false;
  }

  ///
  /// @brief Checks if an AABB intersects any of this vector.
  /// @param other AABB to check intersection with.
  /// @returns `true` if AABBs intersects, `false` otherwise.
  ///
  bool intersects(const AABB<Vec3> &other) const {
    for (const AABB<Vec3> &box : *this)
      if (box.intersects(other))
        return true;
    return false;
  }

  ///
  /// @brief Checks if an AABB vector intersects this one.
  /// @param other AABB vector to check intersection with.
  /// @returns `true` if AABBs intersects, `false` otherwise.
  ///
  bool intersects(const AABBVector<Vec3> &other) const {
    for (const AABB<Vec3> &obox : other)
      for (const AABB<Vec3> &box : *this)
        if (box.intersects(obox))
          return true;
    return false;
  }
};

}

#endif /* DIGGLER_AABB_HPP */
