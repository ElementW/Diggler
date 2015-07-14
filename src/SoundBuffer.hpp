#ifndef SOUND_BUFFER_HPP
#define SOUND_BUFFER_HPP
#include <AL/al.h>
#include <string>

namespace Diggler {

class SoundBuffer {
private:
	bool moved;
	ALuint id;

public:
	SoundBuffer();
	// No copy
	SoundBuffer(const SoundBuffer&) = delete;
	// Move
	SoundBuffer(SoundBuffer&&);
	~SoundBuffer();
	
	void loadOgg(const std::string &path);
	
	operator ALint() const { return id; }
	ALint getId() const { return id; }
};

}

#endif