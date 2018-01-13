#include "Sound.hpp"
#include "Platform.hpp"

namespace diggler {

Sound::Sound(const SoundBuffer *buffer) : buffer(buffer), createdRelative(true) {
  alGenSources(1, &id);
  alSourcei(id, AL_BUFFER, buffer->getId());
  setRelative(true);
  setPosition(glm::vec3(0, 0, 0));
}

Sound::Sound(const SoundBuffer *buffer, bool relative, const glm::vec3 &pos)
  : buffer(buffer), createdRelative(false) {
  alGenSources(1, &id);
  alSourcei(id, AL_BUFFER, buffer->getId());
  setRelative(relative);
  setPosition(pos);
}

Sound::Sound(const Sound &s) {
  alGenSources(1, &id);
  buffer = s.buffer;
  createdRelative = s.createdRelative;
  alSourcei(id, AL_BUFFER, buffer->getId());
  setRelative(s.getRelative());
  setPosition(s.getPosition());
  setPitch(s.getPitch());
  setLooping(s.getLooping());
  setVelocity(s.getVelocity());
  setDirection(s.getDirection());
  setGain(s.getGain());
}

Sound::Sound(Sound&& s) {
  id = s.id;
  buffer = s.buffer;
  s.buffer = nullptr;
  createdRelative = s.createdRelative;
}

Sound::~Sound() {
  if (buffer == nullptr) // Got moved
    return;
  alDeleteSources(1, &id);
}

void Sound::play() {
  alSourcePlay(id);
}

void Sound::stop() {
  alSourceStop(id);
  alSourceRewind(id);
}

bool Sound::isPlaying() const {
  ALint status;
  alGetSourcei(id, AL_SOURCE_STATE, &status);
  return status == AL_PLAYING;
}

float Sound::getGain() const {
  ALfloat value = 1.f;
  alGetSourcef(id, AL_GAIN, &value);
  return value;
}

void Sound::setGain(float value) {
  alSourcef(id, AL_GAIN, value);
}

bool Sound::getRelative() const {
  ALint value = AL_FALSE;
  alGetSourcei(id, AL_SOURCE_RELATIVE, &value);
  return value == AL_TRUE ? true : false;
}

void Sound::setRelative(bool value) {
  createdRelative = false;
  alSourcei(id, AL_SOURCE_RELATIVE, value ? AL_TRUE : AL_FALSE);
}

float Sound::getPitch() const {
  ALfloat value = 1.f;
  alGetSourcef(id, AL_PITCH, &value);
  return value;
}

void Sound::setPitch(float value) {
  alSourcef(id, AL_PITCH, value);
}

bool Sound::getLooping() const {
  ALint value = AL_FALSE;
  alGetSourcei(id, AL_LOOPING, &value);
  return value == AL_TRUE ? true : false;
}

void Sound::setLooping(bool value) {
  alSourcei(id, AL_SOURCE_RELATIVE, value ? AL_TRUE : AL_FALSE);
}

glm::vec3 Sound::getDirection() const {
  ALfloat x = 0.f, y = 0.f, z = 0.f;
  alGetSource3f(id, AL_DIRECTION, &x, &y, &z);
  return glm::vec3(x, y, z);
}

void Sound::setDirection(const glm::vec3 &value) {
  if (createdRelative) setRelative(createdRelative = false);
  alSource3f(id, AL_DIRECTION, value.x, value.y, value.z);
}

glm::vec3 Sound::getPosition() const {
  ALfloat x = 0.f, y = 0.f, z = 0.f;
  alGetSource3f(id, AL_POSITION, &x, &y, &z);
  return glm::vec3(x, y, z);
}

void Sound::setPosition(const glm::vec3 &value) {
  if (createdRelative) setRelative(createdRelative = false);
  alSource3f(id, AL_POSITION, value.x, value.y, value.z);
}

glm::vec3 Sound::getVelocity() const {
  ALfloat x = 0.f, y = 0.f, z = 0.f;
  alGetSource3f(id, AL_VELOCITY, &x, &y, &z);
  return glm::vec3(x, y, z);
}

void Sound::setVelocity(const glm::vec3 &value) {
  if (createdRelative) setRelative(createdRelative = false);
  alSource3f(id, AL_VELOCITY, value.x, value.y, value.z);
}

}