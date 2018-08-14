#include "Sound.hpp"

#include "SoundBuffer.hpp"

namespace diggler {

Sound::Sound(const SoundBuffer *buffer) :
  buffer(buffer) {
  alGenSources(1, &m_id);
  alSourcei(m_id, AL_BUFFER, buffer->id());
  setRelative(true);
  setPosition(glm::vec3(0, 0, 0));
}

Sound::Sound(const SoundBuffer *buffer, bool relative, const glm::vec3 &pos) :
  buffer(buffer) {
  alGenSources(1, &m_id);
  alSourcei(m_id, AL_BUFFER, buffer->id());
  setRelative(relative);
  setPosition(pos);
}

Sound::Sound(const Sound &s) {
  alGenSources(1, &m_id);
  buffer = s.buffer;
  alSourcei(m_id, AL_BUFFER, buffer->id());
  setRelative(s.isRelative());
  setPosition(s.position());
  setPitch(s.pitch());
  setLooping(s.looping());
  setVelocity(s.velocity());
  setDirection(s.direction());
  setGain(s.gain());
}

Sound::Sound(Sound&& s) {
  m_id = s.m_id;
  buffer = s.buffer;
  s.buffer = nullptr;
}

Sound::~Sound() {
  if (buffer == nullptr) // Got moved
    return;
  alDeleteSources(1, &m_id);
}

void Sound::play() {
  alSourcePlay(m_id);
}

void Sound::stop() {
  alSourceStop(m_id);
  alSourceRewind(m_id);
}

bool Sound::isPlaying() const {
  ALint status;
  alGetSourcei(m_id, AL_SOURCE_STATE, &status);
  return status == AL_PLAYING;
}

float Sound::gain() const {
  ALfloat value = 1.f;
  alGetSourcef(m_id, AL_GAIN, &value);
  return value;
}

void Sound::setGain(float value) {
  alSourcef(m_id, AL_GAIN, value);
}

bool Sound::isRelative() const {
  ALint value = AL_FALSE;
  alGetSourcei(m_id, AL_SOURCE_RELATIVE, &value);
  return value == AL_TRUE ? true : false;
}

void Sound::setRelative(bool value) {
  alSourcei(m_id, AL_SOURCE_RELATIVE, value ? AL_TRUE : AL_FALSE);
}

float Sound::pitch() const {
  ALfloat value = 1.f;
  alGetSourcef(m_id, AL_PITCH, &value);
  return value;
}

void Sound::setPitch(float value) {
  alSourcef(m_id, AL_PITCH, value);
}

bool Sound::looping() const {
  ALint value = AL_FALSE;
  alGetSourcei(m_id, AL_LOOPING, &value);
  return value == AL_TRUE ? true : false;
}

void Sound::setLooping(bool value) {
  alSourcei(m_id, AL_SOURCE_RELATIVE, value ? AL_TRUE : AL_FALSE);
}

glm::vec3 Sound::direction() const {
  ALfloat x = 0.f, y = 0.f, z = 0.f;
  alGetSource3f(m_id, AL_DIRECTION, &x, &y, &z);
  return glm::vec3(x, y, z);
}

void Sound::setDirection(const glm::vec3 &value) {
  alSource3f(m_id, AL_DIRECTION, value.x, value.y, value.z);
}

glm::vec3 Sound::position() const {
  ALfloat x = 0.f, y = 0.f, z = 0.f;
  alGetSource3f(m_id, AL_POSITION, &x, &y, &z);
  return glm::vec3(x, y, z);
}

void Sound::setPosition(const glm::vec3 &value) {
  alSource3f(m_id, AL_POSITION, value.x, value.y, value.z);
}

glm::vec3 Sound::velocity() const {
  ALfloat x = 0.f, y = 0.f, z = 0.f;
  alGetSource3f(m_id, AL_VELOCITY, &x, &y, &z);
  return glm::vec3(x, y, z);
}

void Sound::setVelocity(const glm::vec3 &value) {
  alSource3f(m_id, AL_VELOCITY, value.x, value.y, value.z);
}

}
