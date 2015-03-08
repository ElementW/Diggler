#include "LocalPlayer.hpp"
#include "Game.hpp"
#include <cstdio>
#include <limits>
#include <sstream>
#include <glm/gtx/rotate_vector.hpp>
#include "Audio.hpp"
#include "network/NetHelper.hpp"

namespace Diggler {

static float Acceleration = 24.0f;

static float MvmtDamping = 1/24.0f;

static float Gravity = 18.0f; // -Y acceleration (blocks/sec/sec)

static float JumpForce = Gravity/2.8f;

static float MaxSpeed = 6.f;
static float RoadMaxSpeed = 12.f;

static int i(const float &f) {
	if (f >= 0)
		return (int)f;
	return ((int)f)-1;
}

LocalPlayer::LocalPlayer(Game *G) : Player(G), goingForward(false), goingBackward(false), goingLeft(false), goingRight(false),
	hasGravity(true), hasNoclip(false) {
	size = glm::vec3(0.3f, 1.9f, 0.3f);
	eyesPos = glm::vec3(0.f, 1.7f, 0.f);
}

void LocalPlayer::special1() {
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

void LocalPlayer::lookAt(const glm::vec3& at) {
	camera.lookAt(at);
	G->A->updateAngle();
}

void LocalPlayer::update(const float &delta) {
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
	
	// Apply gravity
	if (hasGravity) {
		if (!onGround && velocity.y <= -60) {
			BlockType b = G->SC->get(position.x, position.y-1, position.z);
			onGround = !Blocks::canGoThrough(b, team);
			if (onGround) {
				setDead(true, DeathReason::Fall, true);
				velocity = glm::vec3(0);
				return;
			}
		}
		if (onGround) {
			BlockType b = G->SC->get(position.x, position.y-1, position.z);
			onGround = !Blocks::canGoThrough(b, team);
			onRoad = (b == BlockType::Road);
		}
		if (!onGround)
			velocity.y -= Gravity * delta;
	}
	
	if (!hasNoclip) {
		glm::vec3 velXZ(velocity.x, 0, velocity.z);
		int maxSpeed = (onRoad ? RoadMaxSpeed : MaxSpeed);
		if (glm::length(velXZ) > maxSpeed) {
			velXZ = glm::normalize(velXZ);
			velocity.x = velXZ.x * maxSpeed;
			velocity.z = velXZ.z * maxSpeed;
		}
		if (velocity.y < -80)
			velocity.y = -80;
	}
	
	if (velocity.x > -0.001f && velocity.x < 0.001f) velocity.x = 0.f;
	if (velocity.y > -0.001f && velocity.y < 0.001f) velocity.y = 0.f;
	if (velocity.z > -0.001f && velocity.z < 0.001f) velocity.z = 0.f;
	
	glm::vec3 destPos = position + velocity * delta;
	accel = velocity - initialVel;
	
	if (velocity != glm::vec3(0.f)) { // avoids useless calculus
		if (hasNoclip) {
			position = destPos;
		} else {
			float x = destPos.x, y = destPos.y, z = destPos.z;
			BlockType bTop = G->SC->get(x, y+size.y, z),
					bBottom = G->SC->get(x, y, z);
			if (velocity.y > 0.f)
				if (!Blocks::canGoThrough(bTop, team)) {
					velocity.y = 0.f;
					position.y = (int)(position.y+size.y+1)-size.y;
				}
			if (velocity.y < 0.f)
				if (!Blocks::canGoThrough(bBottom, team)) {
					velocity.y = 0.f;
					y = position.y = (int)(position.y);
					onGround = 1;
				}
			bTop = G->SC->get(x, y+size.y, z);
			bBottom = G->SC->get(x, y-onGround, z);
			BlockType bNTop = G->SC->get(x+size.x, y+size.y, z),
					bNBottom = G->SC->get(x+size.x, y, z),
					
					bSTop = G->SC->get(x-size.x, y+size.y, z),
					bSBottom = G->SC->get(x-size.x, y, z),
					
					bETop = G->SC->get(x, y+size.y, z+size.z),
					bEBottom = G->SC->get(x, y, z+size.z),
					
					bWTop = G->SC->get(x, y+size.y, z-size.z),
					bWBottom = G->SC->get(x, y, z-size.z);
			if (velocity.x > 0.f)
				if (!Blocks::canGoThrough(bNTop, team) || !Blocks::canGoThrough(bNBottom, team)) {
					if (bNTop == BlockType::Lava || bNBottom == BlockType::Lava) {
						setDead(true, DeathReason::Lava, true);
						return;
					}
					velocity.x = 0.f;
				}
			if (velocity.x < 0.f)
				if (!Blocks::canGoThrough(bSTop, team) || !Blocks::canGoThrough(bSBottom, team)) {
					if (bSTop == BlockType::Lava || bSBottom == BlockType::Lava) {
						setDead(true, DeathReason::Lava, true);
						return;
					}
					velocity.x = 0.f;
				}
			if (velocity.z > 0.f)
				if (!Blocks::canGoThrough(bETop, team) || !Blocks::canGoThrough(bEBottom, team)) {
					if (bETop == BlockType::Lava || bEBottom == BlockType::Lava) {
						setDead(true, DeathReason::Lava, true);
						return;
					}
					velocity.z = 0.f;
				}
			if (velocity.z < 0.f)
				if (!Blocks::canGoThrough(bWTop, team) || !Blocks::canGoThrough(bWBottom, team)) {
					if (bWTop == BlockType::Lava || bWBottom == BlockType::Lava) {
						setDead(true, DeathReason::Lava, true);
						return;
					}
					velocity.z = 0.f;
				}
			switch (bTop) {
			case BlockType::Lava:
				setDead(true, DeathReason::Lava, true);
				return;
			case BlockType::Shock:
				setDead(true, DeathReason::Shock, true);
				return;
			default:
				break;
			}
			switch (bBottom) {
			case BlockType::Jump:
				if (G->Time - lastJumpTime > 0.2) {
					velocity.y = JumpForce * 2;
					NetHelper::SendEvent(G, Net::EventType::PlayerJumpOnPad);
					lastJumpTime = G->Time;
				}
				break;

			case BlockType::Lava:
				setDead(true, DeathReason::Lava, true);
				return;

			default:
				break;
			}
			position += velocity * delta;
		}
		
		camera.setPosition(position + eyesPos);
		
		G->A->updatePos();
	}
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

bool LocalPlayer::raytracePointed(glm::ivec3 *pointed, glm::ivec3 *face) {
	return raytracePointed(sqrt(
		(G->SC->getChunksX()*CX)*(G->SC->getChunksX()*CX) +
		(G->SC->getChunksY()*CY)*(G->SC->getChunksY()*CY) +
		(G->SC->getChunksX()*CZ)*(G->SC->getChunksX()*CZ)
	), pointed, face);
}

bool LocalPlayer::raytracePointed(int maxDist, glm::ivec3 *pointed, glm::ivec3 *face, float granularity) {
	glm::vec3 pos = position+eyesPos;
	glm::vec3 rayDir = camera.m_lookAt;
	glm::vec3 delta = rayDir * granularity;
	int x, y, z;
	glm::ivec3 lastTested(pos);
	int cnt = ceil(maxDist / glm::length(delta));
	for (int n = 0; n < cnt; n++) {
		pos += delta;
		x = i(pos.x); y = i(pos.y); z = i(pos.z);
		if (lastTested.x != x || lastTested.y != y || lastTested.z != z) {
			BlockType testBlock = G->SC->get(x, y, z);
			if (testBlock != BlockType::Air) {
				if (pointed)
					*(pointed) = glm::ivec3(x, y, z);
				if (face)
					*(face) = lastTested;
				return true;
			}
			lastTested.x = x; lastTested.y = y; lastTested.z = z;
		}
		
	}
	return false;
}

}