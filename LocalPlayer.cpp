#include "LocalPlayer.hpp"
#include "Game.hpp"
#include <cstdio>
#include <glm/gtx/rotate_vector.hpp>
#include <sstream>
#include "Audio.hpp"
#include "network/NetHelper.hpp"

// I'm shit at player physics, so I kinda copied jMonkey
// https://github.com/jMonkeyEngine/jmonkeyengine/blob/1b0f6d0f59650772bac4588d1733c061ff50d8c8/jme3-bullet/src/common/java/com/jme3/bullet/control/BetterCharacterControl.java

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
	velocity = position = glm::vec3(0.f);
}

void LocalPlayer::special1() {
}

// thx http://martin.ankerl.com/2012/01/25/optimized-approximative-pow-in-c-and-cpp/
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
						setDead(true, DeathReason::Lava);
						return;
					}
					velocity.x = 0.f;
				}
			if (velocity.x < 0.f)
				if (!Blocks::canGoThrough(bSTop, team) || !Blocks::canGoThrough(bSBottom, team)) {
					if (bSTop == BlockType::Lava || bSBottom == BlockType::Lava) {
						setDead(true, DeathReason::Lava);
						return;
					}
					velocity.x = 0.f;
				}
			if (velocity.z > 0.f)
				if (!Blocks::canGoThrough(bETop, team) || !Blocks::canGoThrough(bEBottom, team)) {
					if (bETop == BlockType::Lava || bEBottom == BlockType::Lava) {
						setDead(true, DeathReason::Lava);
						return;
					}
					velocity.z = 0.f;
				}
			if (velocity.z < 0.f)
				if (!Blocks::canGoThrough(bWTop, team) || !Blocks::canGoThrough(bWBottom, team)) {
					if (bWTop == BlockType::Lava || bWBottom == BlockType::Lava) {
						setDead(true, DeathReason::Lava);
						return;
					}
					velocity.z = 0.f;
				}
			switch (bTop) {
				case BlockType::Lava:
					setDead(true, DeathReason::Lava);
					return;
				case BlockType::Shock:
					setDead(true, DeathReason::Shock);
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
					setDead(true, DeathReason::Lava);
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

void LocalPlayer::setDead(bool dead, DeathReason dr) {
	if (deathTime == 0.0) {
		deathTime = G->Time;
		Player::setDead(dead, dr);
	} else {
		if (!dead) {
			deathTime = 0.0;
			deathSent = false;
		}
	}
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


}