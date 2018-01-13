#include "FileStream.hpp"

namespace diggler {
namespace io {

inline std::ios::seekdir getSeekDir(SeekableStream::Whence whence) {
  switch (whence) {
    case SeekableStream::Whence::Set:
      return std::ios::beg;
    case SeekableStream::Whence::Current:
      return std::ios::cur;
    // case SeekableStream::Whence::End:
      // return std::ios::end;
  }
  return std::ios::cur;
}

InFileStream::InFileStream(const std::string &path) {
  strm.open(path);
}

InFileStream::~InFileStream() {
  strm.close();
}

InFileStream::SizeT InFileStream::length() const {
  auto pos = strm.tellg();
  strm.seekg(0, decltype(strm)::end);
  auto size = strm.tellg();
  strm.seekg(pos, decltype(strm)::beg);
  return size;
}

void InFileStream::readData(void *data, SizeT len) {
  strm.read(reinterpret_cast<char*>(data), len);
}

SeekableStream::PosT InFileStream::tell() const {
  return strm.tellg();
}

void InFileStream::seek(OffT off, Whence whence) {
  strm.seekg(off, getSeekDir(whence));
}


OutFileStream::OutFileStream(const std::string &path) {
  strm.open(path);
}

OutFileStream::~OutFileStream() {
  strm.close();
}

void OutFileStream::writeData(const void *data, SizeT len) {
  strm.write(reinterpret_cast<const char*>(data), len);
}

SeekableStream::PosT OutFileStream::tell() const {
  return strm.tellp();
}

void OutFileStream::seek(OffT off, Whence whence) {
  strm.seekp(off, getSeekDir(whence));
}

}
}
