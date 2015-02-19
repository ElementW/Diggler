#ifndef SOUND_HPP
#define SOUND_HPP
#include <string>
#include "SoundBuffer.hpp"
#include <glm/glm.hpp>

namespace Diggler {

class Sound {
private:
	const SoundBuffer *buffer;
	ALuint id; bool createdRelative;

public:
	Sound(const SoundBuffer *buffer);
	Sound(const SoundBuffer *buffer, bool relative, const glm::vec3 &pos = glm::vec3());
	// Copy
	Sound(const Sound&);
	// Move
	Sound(Sound&&);
	~Sound();
	void play();
	void stop();
	bool isPlaying() const;
	inline ALuint getId() const { return id; }
	
	float getGain() const;
	void setGain(float value);
	
	bool getLooping() const;
	void setLooping(bool value);
	
	float getPitch() const;
	void setPitch(float value);
	
	bool getRelative() const;
	void setRelative(bool value);
	
	glm::vec3 getPosition() const;
	void setPosition(const glm::vec3 &value);
	
	glm::vec3 getVelocity() const;
	void setVelocity(const glm::vec3 &value);
	
	glm::vec3 getDirection() const;
	void setDirection(const glm::vec3 &value);
};

}

#endif