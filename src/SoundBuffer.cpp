#include "SoundBuffer.hpp"

#include <stb_vorbis.h>

#include "platform/Types.hpp"
#include "util/Log.hpp"

namespace diggler {

using Util::Log;
using namespace Util::Logging::LogLevels;

static const char *TAG = "SoundBuffer";

inline ALenum alGetFormat(short channels, short samples) {
  bool stereo = (channels > 1);
  switch (samples) {
  case 16:
    if (stereo)
      return AL_FORMAT_STEREO16;
    else
      return AL_FORMAT_MONO16;
  case 8:
    if (stereo)
      return AL_FORMAT_STEREO8;
    else
      return AL_FORMAT_MONO8;
  default:
    return -1;
  }
}

const char* alGetErrorString(ALenum error) {
  switch (error) {
  case AL_NO_ERROR:
    return "AL_NO_ERROR";
  case AL_INVALID_NAME:
    return "AL_INVALID_NAME";
  case AL_INVALID_ENUM:
    return "AL_INVALID_ENUM";
  case AL_INVALID_VALUE:
    return "AL_INVALID_VALUE";
  case AL_INVALID_OPERATION:
    return "AL_INVALID_OPERATION";
  case AL_OUT_OF_MEMORY:
    return "AL_OUT_OF_MEMORY";
  default:
    return "Unknown error";
  }
}

SoundBuffer::SoundBuffer() : m_id(0) {
}

SoundBuffer::SoundBuffer(SoundBuffer &&b) :
  m_id(b.m_id) {
  b.m_id = 0;
}

SoundBuffer& SoundBuffer::operator=(SoundBuffer &&b) {
  if (m_id != 0) {
    alDeleteBuffers(1, &m_id);
  }
  m_id = b.m_id;
  b.m_id = 0;
  return *this;
}

SoundBuffer::~SoundBuffer() {
  if (m_id != 0) {
    alDeleteBuffers(1, &m_id);
  }
}

void SoundBuffer::loadOgg(const std::string &path) {
  int error = 0;
  stb_vorbis *stream = stb_vorbis_open_filename(const_cast<char*>(path.c_str()), &error, nullptr);
  if (stream == nullptr) {
    Log(Error, TAG) << "Could not load " << path << " : " << error;
    return;
  }

  // Get file info
  stb_vorbis_info info = stb_vorbis_get_info(stream);
  ALenum format = alGetFormat(info.channels, 16); // stb_vorbis always 16-bit samples
  uint bufferSize = stb_vorbis_stream_length_in_samples(stream);

  // Create buffer
  ALshort *bufferData = new ALshort[bufferSize];

  // Fill the buffer
  stb_vorbis_get_samples_short_interleaved(stream, info.channels, bufferData, bufferSize);

  if (m_id == 0) {
    alGenBuffers(1, &m_id);
  }

  // Send the buffer data
  alBufferData(m_id, format, bufferData, stb_vorbis_stream_length_in_samples(stream)*sizeof(ALshort), info.sample_rate);
  //getDebugStream() << path << ' ' << info.sample_rate << "Hz" << std::endl;

  delete[] bufferData;
  stb_vorbis_close(stream);
}

}
