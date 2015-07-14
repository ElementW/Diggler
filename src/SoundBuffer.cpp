#include "SoundBuffer.hpp"
#include "Platform.hpp"
#include <stb_vorbis.h>

namespace Diggler {

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

SoundBuffer::SoundBuffer() : moved(false) {
	alGenBuffers(1, &id);
}

SoundBuffer::SoundBuffer(SoundBuffer &&b) {
	id = b.id;
	b.moved = true;
}

SoundBuffer::~SoundBuffer() {
	if (moved)
		return;
	alDeleteBuffers(1, &id);
}

void SoundBuffer::loadOgg(const std::string &path) {
	int error = 0;
	stb_vorbis* stream = stb_vorbis_open_filename(const_cast<char*>(path.c_str()), &error, nullptr);
	if (stream == nullptr) {
		getDebugStream() << "Could not load " << path << " : " << error << std::endl;
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

	// Send the buffer data
	alBufferData(id, format, bufferData, stb_vorbis_stream_length_in_samples(stream)*sizeof(ALshort), info.sample_rate);
	//getDebugStream() << path << ' ' << info.sample_rate << "Hz" << std::endl;

	delete[] bufferData;
	stb_vorbis_close(stream);
}

}