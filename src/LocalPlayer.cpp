#include "LocalPlayer.hpp"

#include <cstdio>
#include <cmath>
#include <limits>
#include <sstream>

#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "AABB.hpp"
#include "Audio.hpp"
#include "Game.hpp"
#include "network/NetHelper.hpp"

namespace Diggler {

static float Acceleration = 18.0f;

static float MvmtDamping = 1/(Acceleration*10.f);

static float Gravity = 18.0f; // -Y acceleration (blocks*sec^-1*sec^-1)

static float JumpForce = Gravity/2.7f;
static float LadderClimbSpeed = 3.f;

static float MaxSpeed = 5.f;
static float RoadMaxSpeed = MaxSpeed*2;

static float MinYVelocity = -80.f;

static float HurtYVelocity = -12.f;
static float LethalYVelocity = -16.f;

LocalPlayer::LocalPlayer(Game *G) : Player(G), goingForward(false), goingBackward(false), goingLeft(false), goingRight(false),
  hasGravity(true), hasNoclip(false), health(1) {
  extents = glm::vec3(0.3f, 0.75f, 0.3f);
  eyesPos = glm::vec3(0.f, 0.55f, 0.f);
}

// Thanks http://martin.ankerl.com/2012/01/25/optimized-approximative-pow-in-c-and-cpp/
// FIXME: not all platforms use IEEE754
inline double fpow(double a, double b) {
  union {
    double d;
    int x[2];
  } u = { a };
  u.x[1] = (int)(b * (u.x[1] - 1072632447) + 1072632447);
  u.x[0] = 0;
  return u.d;
}

void LocalPlayer::lookAt(const glm::vec3 &at) {
  camera.lookAt(at);
  G->A->updateAngle();
}

void LocalPlayer::update(float delta) {
  health += delta/5;
  if (health >= 1)
    health = 1;

  bool moving = goingForward || goingBackward || goingLeft || goingRight;
  glm::vec3 initialVel = velocity;
  if (!moving) {
    float finalDamp = fpow(MvmtDamping, delta);
    velocity.x *= finalDamp;
    velocity.z *= finalDamp;
    if (!this->hasGravity)
      velocity.y *= finalDamp;
  }

  // Apply player's will movement
  glm::vec3 normMove = glm::normalize(glm::vec3(camera.m_lookAt.x, camera.m_lookAt.y * !hasGravity, camera.m_lookAt.z));
  float acceleration = Acceleration*delta;
  if (goingForward) {
    velocity += acceleration * normMove;
  }
  if (goingBackward) {
    velocity -= acceleration * normMove;
  }
  if (goingLeft) {
    velocity += acceleration * glm::rotateY(glm::normalize(glm::vec3(normMove.x, 0, normMove.z)), (float)M_PI/2);
  }
  if (goingRight) {
    velocity -= acceleration * glm::rotateY(glm::normalize(glm::vec3(normMove.x, 0, normMove.z)), (float)M_PI/2);
  }

  glm::ivec3 &min = aabbmin, &max = aabbmax;
  min.x = floor(position.x - extents.x + std::min(0.f, velocity.x*delta));
  min.y = floor(position.y - extents.y + std::min(0.f, velocity.y*delta));
  min.z = floor(position.z - extents.z + std::min(0.f, velocity.z*delta));
  max.x =  ceil(position.x + extents.x + std::max(0.f, velocity.x*delta));
  max.y =  ceil(position.y + extents.y + std::max(0.f, velocity.y*delta));
  max.z =  ceil(position.z + extents.z + std::max(0.f, velocity.z*delta));

#if 1
  // Apply gravity
  if (hasGravity) {
    /* setDead(true, DeathReason::Fall, true);
        G->A->playSound("hitground");*/
    velocity.y -= Gravity * delta;
  }

  /*if (!hasNoclip) {
    glm::vec3 velXZ(velocity.x, 0, velocity.z);
    int maxSpeed = (onRoad ? RoadMaxSpeed : MaxSpeed);
    if (glm::length(velXZ) > maxSpeed) {
      velXZ = glm::normalize(velXZ);
      velocity.x = velXZ.x * maxSpeed;
      velocity.z = velXZ.z * maxSpeed;
    }
    if (velocity.y < MinYVelocity)
      velocity.y = MinYVelocity;
  }*/
#endif
  if (velocity.x > -.001f && velocity.x < .001f) velocity.x = 0.f;
  if (velocity.y > -.001f && velocity.y < .001f) velocity.y = 0.f;
  if (velocity.z > -.001f && velocity.z < .001f) velocity.z = 0.f;

  glm::vec3 destPos = position + velocity * delta;
  accel = velocity - initialVel;

  if (velocity != glm::vec3()) { // avoids useless calculus
    if (hasNoclip) {
      position = destPos;
    }
#if 1
    else {
      float pdelta = 1.0f; glm::vec3 pnorm;
      auto dtvel = velocity * delta;
      AABB<> plrBox(
        glm::vec3(position.x - extents.x, position.y - extents.y, position.z - extents.z),
        glm::vec3(position.x + extents.x, position.y + extents.y, position.z + extents.z)
      );
      AABB<> blockBox;
      for (int cx = min.x; cx < max.x; ++cx) {
        blockBox.v1.x = cx; blockBox.v2.x = cx + 1;
        for (int cy = min.y; cy < max.y; ++cy) {
          blockBox.v1.y = cy; blockBox.v2.y = cy + 1;
          for (int cz = min.z; cz < max.z; ++cz) {
            blockBox.v1.z = cz; blockBox.v2.z = cz + 1;
            BlockId id = W->getBlockId(cx, cy, cz);
            if (id != Content::BlockAirId) {
              glm::vec3 normal;
              float d = plrBox.sweptCollision(blockBox, dtvel.x, dtvel.y, dtvel.z, normal.x, normal.y, normal.z);
              if (d < pdelta) {
                pdelta = d;
                pnorm = normal;
              }
              /*if (pdelta == 0.f) {
                onGround = true;
              }*/
            }
          }
        }
      }
      if (pdelta != 1.0f) {
        float remainingtime = 1.f - pdelta;
        float dotprod = glm::dot(velocity, pnorm) * remainingtime;
        velocity.x = pnorm.y * pnorm.z * dotprod;
        //velocity.y = pnorm.x * pnorm.z * dotprod;
        velocity.z = pnorm.x * pnorm.y * dotprod;
        getDebugStream() << dotprod << ' ' << pdelta << std::endl;
      }
      //getDebugStream() << "--" << std::endl;
      velocity *= pdelta;
      position += velocity * delta;
    }
#endif
    camera.setPosition(position + eyesPos);
    G->A->updatePos();
  }
}

void LocalPlayer::render(const glm::mat4 &transform) const {
  const Program *P = G->PM->getProgram(PM_3D | PM_COLORED);
  P->bind();
  glEnableVertexAttribArray(P->att("coord"));
  glEnableVertexAttribArray(P->att("color"));
  glUniformMatrix4fv(P->uni("mvp"), 1, GL_FALSE, glm::value_ptr(transform));
  static Render::gl::VBO vbo;
  const glm::ivec3 &min = aabbmin, &max = aabbmax;
  struct Coord { int x, y, z; uint8 r, g, b; } pts[] = {
    { min.x, min.y, min.z, 0, 1, 0 },
    { max.x, min.y, min.z, 0, 1, 0 },
    { min.x, min.y, min.z, 0, 1, 0 },
    { min.x, max.y, min.z, 0, 1, 0 },
    { min.x, min.y, min.z, 0, 1, 0 },
    { min.x, min.y, max.z, 0, 1, 0 },
    { max.x, min.y, min.z, 0, 1, 0 },
    { max.x, max.y, min.z, 0, 1, 0 },
    { max.x, min.y, min.z, 0, 1, 0 },
    { max.x, min.y, max.z, 0, 1, 0 },
    { min.x, max.y, min.z, 0, 1, 0 },
    { max.x, max.y, min.z, 0, 1, 0 },

    { max.x, max.y, max.z, 0, 1, 0 },
    { min.x, max.y, max.z, 0, 1, 0 },
    { max.x, max.y, max.z, 0, 1, 0 },
    { max.x, min.y, max.z, 0, 1, 0 },
    { max.x, max.y, max.z, 0, 1, 0 },
    { max.x, max.y, min.z, 0, 1, 0 },
    { min.x, max.y, max.z, 0, 1, 0 },
    { min.x, min.y, max.z, 0, 1, 0 },
    { min.x, max.y, max.z, 0, 1, 0 },
    { min.x, max.y, min.z, 0, 1, 0 },
    { max.x, min.y, max.z, 0, 1, 0 },
    { min.x, min.y, max.z, 0, 1, 0 },
  };
  vbo.setDataKeepSize(pts, sizeof(pts)/sizeof(Coord), GL_STREAM_DRAW);
  vbo.bind();
  glVertexAttribPointer(P->att("coord"), 3, GL_INT, GL_FALSE, sizeof(Coord), 0);
  glVertexAttribPointer(P->att("color"), 3, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(Coord), (GLvoid*)(offsetof(Coord, r)));
  glDrawArrays(GL_LINES, 0, sizeof(pts)/sizeof(Coord));
  glDisableVertexAttribArray(P->att("color"));
  glDisableVertexAttribArray(P->att("coord"));
}

void LocalPlayer::forceCameraUpdate() {
  camera.setPosition(position + eyesPos);
}

void LocalPlayer::setDead(bool dead, DeathReason dr, bool send) {
  if (dead) {
    if (isAlive) {
      deathShown = false;
      deathSent = !send;
      deathTime = G->Time;
    }
  } else {
    deathTime = 0.0;
    deathShown = false;
    deathSent = false;
  }
  Player::setDead(dead, dr, send);
}

void LocalPlayer::goForward(bool enable) {
  goingForward = enable;
}
void LocalPlayer::goBackward(bool enable) {
  goingBackward = enable;
}
void LocalPlayer::goLeft(bool enable) {
  goingLeft = enable;
}
void LocalPlayer::goRight(bool enable) {
  goingRight = enable;
}

void LocalPlayer::setHasGravity(bool fall) {
  hasGravity = fall;
  onGround = false;
}

void LocalPlayer::setHasNoclip(bool fly) {
  hasNoclip = fly;
  setHasGravity(!fly);
}

void LocalPlayer::jump() {
  if (!onGround)
    return;
  onGround = false;
  velocity.y += JumpForce;
}

bool LocalPlayer::raytracePointed(glm::ivec3 *pointed, glm::ivec3 *facing) {
  // TODO: renderdistance
  return raytracePointed(CX*2, pointed, facing);
}

bool LocalPlayer::raytracePointed(float range, glm::ivec3 *pointed, glm::ivec3 *facing) {
  return W->raytrace(position+eyesPos, camera.m_lookAt, range, pointed, facing);
}

}
