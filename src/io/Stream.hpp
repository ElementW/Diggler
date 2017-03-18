#ifndef DIGGLER_IO_STREAM_HPP
#define DIGGLER_IO_STREAM_HPP

#include <goodform/io.hpp>

#include "../Platform.hpp"

namespace Diggler {
namespace IO {

class Stream {
public:
  using SizeT = uint64;
  using PosT = SizeT;
  using OffT = int64;

  virtual ~Stream();

  virtual bool eos() const = 0;
};

class TellableStream : public virtual Stream {
public:
  virtual PosT tell() const = 0;
};

class SeekableStream : public virtual TellableStream {
public:
  enum Whence {
    Set,
    Current,
    // End
  };

  virtual void seek(OffT, Whence = Set) = 0;
  inline void seek(PosT pos) {
    seek(static_cast<OffT>(pos), Whence::Set);
  }

  inline void rewind() {
    seek(static_cast<OffT>(0));
  }
};

class SizedStream : public virtual TellableStream {
public:
  virtual bool eos() const override {
    return remaining() == 0;
  }

  virtual SizeT length() const = 0;
  virtual SizeT remaining() const {
    return length() - tell();
  }
};

class InStream : public virtual Stream, public goodform::istream {
public:
  virtual std::string readString();
  virtual std::u32string readString32();
  virtual bool readBool();
  virtual int64 readI64();
  virtual uint64 readU64();
  virtual int32 readI32();
  virtual uint32 readU32();
  virtual int16 readI16();
  virtual uint16 readU16();
  virtual int8 readI8();
  virtual uint8 readU8();
  virtual float readFloat();
  virtual double readDouble();
  virtual void readData(void *data, SizeT len) = 0;
  virtual void skip(SizeT len);

  // goodform::msgpack compatibility
  bool good() {
    return not eos();
  }
  goodform::istream& read(char *data, size_t size) {
    readData(data, static_cast<SizeT>(size));
    return *this;
  }
};

class InSeekableStream : public virtual InStream, public virtual SeekableStream {
public:
  virtual void skip(SizeT len) {
    seek(len, Whence::Current);
  }
};

class OutStream : public virtual Stream, public goodform::ostream {
public:
  virtual void writeString(const std::string &str);
  virtual void writeString32(const std::u32string &str);
  virtual void writeBool(bool v);
  virtual void writeI64(int64 i);
  virtual void writeU64(uint64 i);
  virtual void writeI32(int32 i);
  virtual void writeU32(uint32 i);
  virtual void writeI16(int16 i);
  virtual void writeU16(uint16 i);
  virtual void writeI8(int8 i);
  virtual void writeU8(uint8 i);
  virtual void writeFloat(float f);
  virtual void writeDouble(double d);
  virtual void writeData(const void *data, SizeT len) = 0;

  // goodform::msgpack compatibility
  bool good() {
    return true;
  }
  goodform::ostream& write(const char *data, size_t size) {
    writeData(data, static_cast<SizeT>(size));
    return *this;
  }
};

class OutSeekableStream : public virtual OutStream, public virtual SeekableStream {
};

}
}

#endif /* DIGGLER_IO_STREAM_HPP */
