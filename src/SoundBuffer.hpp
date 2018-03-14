#ifndef DIGGLER_SOUND_BUFFER_HPP
#define DIGGLER_SOUND_BUFFER_HPP

#include <string>

#include "platform/BuildInfo.hpp"
#ifdef BUILDINFO_PLATFORM_MAC
#include <OpenAL/al.h>
#else
#include <AL/al.h>
#endif

namespace diggler {

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

#endif /* DIGGLER_SOUND_BUFFER_HPP */
