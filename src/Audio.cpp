#include "Audio.hpp"

#include <cstring>
#include <sstream>

#include <AL/al.h>
#include <AL/alext.h>

#include "Game.hpp"
#include "GlobalProperties.hpp"
#include "LocalPlayer.hpp"
#include "Sound.hpp"

#define AUDIO_GC_DEBUG 0

namespace Diggler {

Audio::Audio(Game &G) : G(G), sounds(m_sounds) {
  if (!GlobalProperties::IsSoundEnabled)
    return;

  ALboolean enumeration = alcIsExtensionPresent(nullptr, "ALC_ENUMERATION_EXT");
  if (enumeration == AL_FALSE) {
    getDebugStream() <<
      "OpenAL device enumeration unsupported. The default device will be used." << std::endl;
  } else {
    const ALCchar *devices = alcGetString(nullptr, ALC_DEVICE_SPECIFIER);
    const ALCchar *device = devices, *next = devices + 1;
    std::ostringstream oss;
    oss << "OpenAL devices: ";
    while (device && *device != '\0' && next && *next != '\0') {
      oss << '"' << device << "\", ";
      size_t len = strlen(device);
      device += (len + 1);
      next += (len + 2);
    }
    getDebugStream() << oss.str() << std::endl;
  }

  const ALCchar *deviceName = nullptr;
  m_audioDevice = alcOpenDevice(deviceName);
  if (!m_audioDevice) {
    getDebugStream() << "Failed opening AL device '" << deviceName << "': " << alcGetError(m_audioDevice) << std::endl;
    GlobalProperties::IsSoundEnabled = false;
  }
  deviceName = alcGetString(m_audioDevice, ALC_DEVICE_SPECIFIER);

  int alMajor, alMinor;
  alcGetIntegerv(nullptr, ALC_MAJOR_VERSION, 1, &alMajor);
  alcGetIntegerv(nullptr, ALC_MINOR_VERSION, 1, &alMinor);
  getDebugStreamRaw() << "OpenAL " << alMajor << '.' << alMinor << " -- Device '" << deviceName <<
    '\'' << std::endl;

  ALCint attrs[] = {
    0, 0
  };

  m_audioContext = alcCreateContext(m_audioDevice, attrs);
  if (!alcMakeContextCurrent(m_audioContext)) {
    getDebugStream() << "Failed setting context on AL device '" << deviceName << "': " << alcGetError(m_audioDevice) << std::endl;
    alcCloseDevice(m_audioDevice);
    GlobalProperties::IsSoundEnabled = false;
  }
}

Audio::~Audio() {
  if (!GlobalProperties::IsSoundEnabled)
    return;

  alcMakeContextCurrent(nullptr);
  alcDestroyContext(m_audioContext);
  alcCloseDevice(m_audioDevice);
}

void Audio::updateAngle() {
  const glm::vec3
    &at = G.LP->camera.getLookAt(),
    &up = G.LP->camera.getUp();
  ALfloat plrOrient[6];
  plrOrient[0] = at.x;
  plrOrient[1] = at.y;
  plrOrient[2] = at.z;
  plrOrient[3] = up.x;
  plrOrient[4] = up.y;
  plrOrient[5] = up.z;
  alListenerfv(AL_ORIENTATION, plrOrient);
}

void Audio::updatePos() {
  const glm::vec3
    &pos = G.LP->position,
    &vel = G.LP->velocity;
  ALfloat float3data[3];
  float3data[0] = pos.x;
  float3data[1] = pos.y;
  float3data[2] = pos.z;
  alListenerfv(AL_POSITION, float3data);
  float3data[0] = vel.x;
  float3data[1] = vel.y;
  float3data[2] = vel.z;
  alListenerfv(AL_VELOCITY, float3data);
}

void Audio::update() {
  updatePos();
  updateAngle();
}

void Audio::loadSoundAssets() {
  std::string soundsDir = getAssetsDirectory() + "/sounds/";
  for (const std::string &fn : fs::getFiles(soundsDir)) {
    if (fn.length() >= 4 && fn.substr(fn.length()-4) == ".ogg") {
      addSound(fn.substr(0, fn.length()-4), soundsDir+fn);
    }
  }
}

void Audio::addSound(const std::string &name, const std::string &path) {
  m_sounds.emplace(std::piecewise_construct,
    std::forward_as_tuple(name), std::forward_as_tuple()).first->second.loadOgg(path);
}

void Audio::playSound(const std::string &name) {
  playSound(m_sounds.at(name));
}

void Audio::playSound(const SoundBuffer &buf) {
  m_playing.emplace_back(&buf);
  m_playing.back().play();
  gc();
}

void Audio::playSound(const std::string &name, const glm::vec3 &pos) {
  playSound(m_sounds.at(name), pos);
}

void Audio::playSound(const SoundBuffer &buf, const glm::vec3& pos) {
  m_playing.emplace_back(&buf, false, pos);
  m_playing.back().play();
  gc();
}

/// Garbage Collects the sound sources
void Audio::gc() {
#if AUDIO_GC_DEBUG
  uint freed = 0;
#endif
  std::list<Sound>::const_iterator it = m_playing.begin();
  while (it != m_playing.end()) {
    if (!it->isPlaying()) {
      it = m_playing.erase(it);
#if AUDIO_GC_DEBUG
      freed++;
#endif
    } else {
      ++it;
    }
  }
#if AUDIO_GC_DEBUG
  getDebugStream() << "GC'd " << freed << " sources" << std::endl;
#endif
}

}
