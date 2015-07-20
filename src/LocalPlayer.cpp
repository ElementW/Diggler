#include "LocalPlayer.hpp"
#include <cstdio>
#include <limits>
#include <sstream>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "Audio.hpp"
#include "Game.hpp"
#include "network/NetHelper.hpp"

namespace Diggler {

static float Acceleration = 18.0f;

static float MvmtDamping = 1/(Acceleration*10.f);

static float Gravity = 18.0f; // -Y acceleration (blocks/sec/sec)

static float JumpForce = Gravity/2.7f;
static float LadderClimbSpeed = 3.f;

static float MaxSpeed = 5.f;
static float RoadMaxSpeed = MaxSpeed*2;

static float MinYVelocity = -80.f;

static float HurtYVelocity = -12.f;
static float LethalYVelocity = -16.f;

LocalPlayer::LocalPlayer(Game *G) : Player(G), goingForward(false), goingBackward(false), goingLeft(false), goingRight(false),
	hasGravity(true), hasNoclip(false), health(1) {
	size = glm::vec3(0.3f, 1.5f, 0.3f);
	eyesPos = glm::vec3(0.f, 1.3f, 0.f);
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

	// FIXME!!!! TODO TODO TODO REMAKE
#if 0
	// Apply gravity
	if (hasGravity) {
		if (!onGround && velocity.y <= HurtYVelocity) {
			BlockId b = W->get(position.x, position.y-1, position.z);
			onGround = !Blocks::canGoThrough(b, team) && (b != BlockType::Jump);
			if (onGround) {
				if (velocity.y <= LethalYVelocity) {
					setDead(true, DeathReason::Fall, true);
					position.y = (int)(position.y);
					velocity = glm::vec3(0);
					camera.setPosition(position + eyesPos);
					G->A->updatePos();
					health = 0;
					return;
				}
				G->A->playSound("hitground");
				health -= (velocity.y-HurtYVelocity)/(LethalYVelocity-HurtYVelocity);
				if (health < 0)
					setDead(true, DeathReason::Fall, true);
			}
		}
		if (onGround) {
			BlockType b = G->SC->get(position.x, position.y-1, position.z);
			onGround = !Blocks::canGoThrough(b, team);
			if (onRoad) {
				onRoad = (!onGround || b == BlockType::Road || b == BlockType::Jump);
			} else {
				onRoad = (b == BlockType::Road);
			}
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
		if (velocity.y < MinYVelocity)
			velocity.y = MinYVelocity;
	}
#endif
	if (velocity.x > -0.001f && velocity.x < 0.001f) velocity.x = 0.f;
	if (velocity.y > -0.001f && velocity.y < 0.001f) velocity.y = 0.f;
	if (velocity.z > -0.001f && velocity.z < 0.001f) velocity.z = 0.f;

	glm::vec3 destPos = position + velocity * delta;
	accel = velocity - initialVel;

	if (velocity != glm::vec3(0.f)) { // avoids useless calculus
		if (hasNoclip) {
			position = destPos;
		}
#if 0
		else {
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
					if (bNTop == BlockType::Ladder || bNBottom == BlockType::Ladder) {
						velocity.y = LadderClimbSpeed;
						velocity.z *= 0.75f;
					}
					velocity.x = 0.f;
				}
			if (velocity.x < 0.f)
				if (!Blocks::canGoThrough(bSTop, team) || !Blocks::canGoThrough(bSBottom, team)) {
					if (bSTop == BlockType::Lava || bSBottom == BlockType::Lava) {
						setDead(true, DeathReason::Lava, true);
						return;
					}
					if (bSTop == BlockType::Ladder || bSBottom == BlockType::Ladder) {
						velocity.y = LadderClimbSpeed;
						velocity.z *= 0.75f;
					}
					velocity.x = 0.f;
				}
			if (velocity.z > 0.f)
				if (!Blocks::canGoThrough(bETop, team) || !Blocks::canGoThrough(bEBottom, team)) {
					if (bETop == BlockType::Lava || bEBottom == BlockType::Lava) {
						setDead(true, DeathReason::Lava, true);
						return;
					}
					if (bETop == BlockType::Ladder || bEBottom == BlockType::Ladder) {
						velocity.y = LadderClimbSpeed;
						velocity.x *= 0.75f;
					}
					velocity.z = 0.f;
				}
			if (velocity.z < 0.f)
				if (!Blocks::canGoThrough(bWTop, team) || !Blocks::canGoThrough(bWBottom, team)) {
					if (bWTop == BlockType::Lava || bWBottom == BlockType::Lava) {
						setDead(true, DeathReason::Lava, true);
						return;
					}
					if (bWTop == BlockType::Ladder || bWBottom == BlockType::Ladder) {
						velocity.y = LadderClimbSpeed;
						velocity.x *= 0.75f;
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
					velocity.y = JumpForce * 1.6f;
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
#endif
		camera.setPosition(position + eyesPos);
		G->A->updatePos();
	}
}

void LocalPlayer::render(const glm::mat4 &transform) const {
#if 0
	const Program *P = G->PM->getProgram(PM_3D | PM_COLORED);
	P->bind();
	glEnableVertexAttribArray(P->att("coord"));
	glEnableVertexAttribArray(P->att("color"));
	glUniformMatrix4fv(P->uni("mvp"), 1, GL_FALSE, glm::value_ptr(transform));
	static VBO vbo;
	struct { float x, y, z, r, g, b; } pts[] = {
		(int)position.x+.5f, (int)(position.y+size.y)+.5f, (int)(position.z+size.z)+.5f, 1.f, 0.f, 0.f,
		(int)position.x+.5f, (int)(position.y+size.y)+.5f, (int)position.z+.5f, 0.f, 1.f, 0.f
	};
	vbo.setData(pts, 2, GL_STREAM_DRAW);
	vbo.bind();
	glPointSize(4.f);
	glVertexAttribPointer(P->att("coord"), 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), 0);
	glVertexAttribPointer(P->att("color"), 3, GL_FLOAT, GL_FALSE, 6*sizeof(float), (GLvoid*)(3*sizeof(float)));
	glDrawArrays(GL_POINTS, 0, 2);
	glDisableVertexAttribArray(P->att("color"));
	glDisableVertexAttribArray(P->att("coord"));
#endif
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
	// TODO: renderdistance
	return raytracePointed(CX*2, pointed, face);
}

bool LocalPlayer::raytracePointed(int maxDist, glm::ivec3 *pointed, glm::ivec3 *face, float granularity) {
	glm::vec3 pos = position+eyesPos;
	glm::vec3 rayDir = camera.m_lookAt;
	glm::vec3 delta = rayDir * granularity;
	int x, y, z;
	// TODO: make a floor function for vectors
	glm::ivec3 lastTested(floor(pos.x), floor(pos.y), floor(pos.z));
	int cnt = ceil(maxDist / glm::length(delta));
	for (int n = 0; n < cnt; n++) {
		pos += delta;
		x = floor(pos.x); y = floor(pos.y); z = floor(pos.z);
		if (lastTested.x != x || lastTested.y != y || lastTested.z != z) {
			// TODO: AABBs and non-colliding
			BlockId testBlock = W->getBlockId(x, y, z);
			if (testBlock != Content::BlockAirId) {
				if (pointed)
					*pointed = glm::ivec3(x, y, z);
				if (face)
					*face = lastTested;
				return true;
			}
			lastTested.x = x; lastTested.y = y; lastTested.z = z;
		}
		
	}
	return false;
}

}