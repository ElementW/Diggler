#ifndef LOCAL_PLAYER_HPP
#define LOCAL_PLAYER_HPP
#include "Camera.hpp"
#include "Platform.hpp"
#include "ui/Text.hpp"
#include "Player.hpp"

namespace Diggler {

class Game;

class LocalPlayer : public Player {
private:
	bool goingForward, goingBackward, goingLeft, goingRight;
	bool hasGravity, hasNoclip, onGround, onRoad;

	// Fixes
	double lastJumpTime = 0.0;

public:
	UI::Text *t;
	Camera camera;
	AABB bounds;
	glm::vec3 size, eyesPos;
	
	LocalPlayer(Game *G);
	
	inline glm::mat4 getPVMatrix() { return camera.getPVMatrix(); }
	void lookAt(const glm::vec3 &at);
	inline void setProjection(const glm::mat4 &p) { camera.setProjection(p); }
	void update(const float &delta);
	void forceCameraUpdate();
	void goForward(bool enable);
	void goBackward(bool enable);
	void goLeft(bool enable);
	void goRight(bool enable);
	void setHasGravity(bool fall);
	void setHasNoclip(bool fly);
	void jump();
	void kill(DeathReason dr);
	void special1();
};

}

#endif