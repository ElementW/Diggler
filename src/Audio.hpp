#ifndef DIGGLER_AUDIO_HPP
#define DIGGLER_AUDIO_HPP

#include <map>
#include <list>

#include "platform/BuildInfo.hpp"
#ifdef BUILDINFO_PLATFORM_MACOS
#include <OpenAL/alc.h>
#else
#include <AL/alc.h>
#endif

#include "platform/types/vec3.hpp"
#include "Sound.hpp"
#include "SoundBuffer.hpp"

namespace diggler {

class Game;

class Audio {
private:
  Game &G;
  std::map<std::string, SoundBuffer> m_sounds;
  std::list<Sound> m_playing;
  void gc();
  ALCdevice *m_audioDevice;
  ALCcontext *m_audioContext;

public:
  Audio(Game &G);
  ~Audio();
  
  const std::map<std::string, SoundBuffer> &sounds;
  
  void loadSoundAssets();
  
  void addSound(const std::string &name, const std::string &path);
  void updatePos();
  void updateAngle();
  // Runs updatePos and updateAngle
  void update();
  void playSound(const std::string &name);
  void playSound(const SoundBuffer &buf);
  void playSound(const std::string &name, const vec3 &pos);
  void playSound(const SoundBuffer &buf, const vec3 &pos);
};

}

#endif /* DIGGLER_AUDIO_HPP */
