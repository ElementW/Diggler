#ifndef DIGGLER_SOUND_HPP
#define DIGGLER_SOUND_HPP

#include "platform/BuildInfo.hpp"
#ifdef BUILDINFO_PLATFORM_MAC
#include <OpenAL/al.h>
#else
#include <AL/al.h>
#endif

#include <glm/vec3.hpp>

namespace diggler {

class SoundBuffer;

class Sound {
private:
  const SoundBuffer *buffer;
  ALuint m_id;

public:
  Sound(const SoundBuffer *buffer);
  Sound(const SoundBuffer *buffer, bool relative, const glm::vec3 &pos = glm::vec3());

  Sound(const Sound&);
  Sound(Sound&&);

  ~Sound();

  void play();
  void stop();
  bool isPlaying() const;
  inline ALuint id() const { return m_id; }

  float gain() const;
  void setGain(float value);

  bool looping() const;
  void setLooping(bool value);

  float pitch() const;
  void setPitch(float value);

  bool isRelative() const;
  void setRelative(bool value);

  glm::vec3 position() const;
  void setPosition(const glm::vec3 &value);

  glm::vec3 velocity() const;
  void setVelocity(const glm::vec3 &value);

  glm::vec3 direction() const;
  void setDirection(const glm::vec3 &value);
};

}

#endif /* DIGGLER_SOUND_HPP */
