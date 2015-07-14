#include "FileStream.hpp"

namespace Diggler {

inline std::ios::seekdir getSeekDir(SeekableStream::Whence whence) {
	switch (whence) {
		case SeekableStream::Whence::Begin:
			return std::ios::beg;
		case SeekableStream::Whence::Set:
			return std::ios::cur;
		case SeekableStream::Whence::End:
			return std::ios::end;
	}
	return std::ios::cur;
}

InFileStream::InFileStream(const std::string &path) {
	strm.open(path);
}

InFileStream::~InFileStream() {
	strm.close();
}

void InFileStream::readData(void *data, int len) {
	strm.read(reinterpret_cast<char*>(data), len);
}

SeekableStream::PosT InFileStream::tell() {
	return strm.tellg();
}

void InFileStream::seek(SeekableStream::PosT pos, SeekableStream::Whence whence) {
	strm.seekg(pos, getSeekDir(whence));
}


OutFileStream::OutFileStream(const std::string &path) {
	strm.open(path);
}

OutFileStream::~OutFileStream() {
	strm.close();
}

void OutFileStream::writeData(const void *data, int len) {
	strm.write(reinterpret_cast<const char*>(data), len);
}

SeekableStream::PosT OutFileStream::tell() {
	return strm.tellp();
}

void OutFileStream::seek(SeekableStream::PosT pos, SeekableStream::Whence whence) {
	strm.seekp(pos, getSeekDir(whence));
}

}