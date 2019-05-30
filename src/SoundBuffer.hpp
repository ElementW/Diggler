#ifndef DIGGLER_SOUND_BUFFER_HPP
#define DIGGLER_SOUND_BUFFER_HPP

#include <string>

#include "platform/BuildInfo.hpp"
#ifdef BUILDINFO_PLATFORM_MACOS
#include <OpenAL/al.h>
#else
#include <AL/al.h>
#endif

namespace diggler {

class SoundBuffer {
private:
  ALuint m_id;

public:
  SoundBuffer();

  SoundBuffer(const SoundBuffer&) = delete;
  SoundBuffer(SoundBuffer&&);
  SoundBuffer& operator=(const SoundBuffer&) = delete;
  SoundBuffer& operator=(SoundBuffer&&);

  ~SoundBuffer();
  
  void loadOgg(const std::string &path);
  
  operator ALint() const { return m_id; }
  ALint id() const { return m_id; }
};

}

#endif /* DIGGLER_SOUND_BUFFER_HPP */
