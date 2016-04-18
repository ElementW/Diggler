#ifndef STREAM_HPP
#define STREAM_HPP
#include "../Platform.hpp"

namespace Diggler {

class Stream {};

class SeekableStream : public Stream {
public:
	using PosT = uint64;
	enum Whence {
		Begin,
		Set,
		End
	};

	virtual PosT tell() = 0;
	virtual void seek(PosT, Whence = Set) = 0;
};

class InStream : public Stream {
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
	virtual void readData(void *data, int len) = 0;
};

class OutStream : public Stream {
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
	virtual void writeData(const void *data, int len) = 0;
};

}

#endif
