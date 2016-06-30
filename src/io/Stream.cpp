#include "Stream.hpp"
#include <stdexcept>

namespace Diggler {

void OutStream::writeString(const std::string &str) {
  if (str.size() > UINT16_MAX)
    throw std::length_error("String too long");
  int len = str.size();
  writeU16(len);
  writeData(str.c_str(), len);
}
std::string InStream::readString() {
  uint16 len = readU16();
  char *data = new char[len];
  readData(data, len);
  std::string str(data, len);
  delete[] data;
  return str;
}

void OutStream::writeBool(bool v) {
  writeU8(v);
}
bool InStream::readBool() {
  return readU8();
}

void OutStream::writeString32(const std::u32string &str) {
  if (str.size() > UINT16_MAX)
    throw std::length_error("String too long");
  int len = str.size();
  writeU16(len);
  writeData(str.c_str(), len*sizeof(char32));
}
std::u32string InStream::readString32() {
  uint16 len = readU16();
  char32 *data = new char32[len];
  readData(data, len*sizeof(char32));
  std::u32string str(data, len);
  delete[] data;
  return str;
}

void OutStream::writeI64(int64 i) {
  writeData(&i, 8);
}
int64 InStream::readI64() {
  int64 val;
  readData(&val, 8);
  return val;
}

void OutStream::writeU64(uint64 i) {
  writeData(&i, 8);
}
uint64 InStream::readU64() {
  uint64 val;
  readData(&val, 8);
  return val;
}

void OutStream::writeI32(int32 i) {
  writeData(&i, 4);
}
int32 InStream::readI32() {
  int32 val;
  readData(&val, 4);
  return val;
}

void OutStream::writeU32(uint32 i) {
  writeData(&i, 4);
}
uint32 InStream::readU32() {
  uint32 val;
  readData(&val, 4);
  return val;
}

void OutStream::writeI16(int16 i) {
  writeData(&i, 2);
}
int16 InStream::readI16() {
  int16 val;
  readData(&val, 2);
  return val;
}

void OutStream::writeU16(uint16 i) {
  writeData(&i, 2);
}
uint16 InStream::readU16() {
  uint16 val;
  readData(&val, 2);
  return val;
}

void OutStream::writeI8(int8 i) {
  writeData(&i, 1);
}
int8 InStream::readI8() {
  int8 val;
  readData(&val, 1);
  return val;
}

void OutStream::writeU8(uint8 i) {
  writeData(&i, 1);
}
uint8 InStream::readU8() {
  uint8 val;
  readData(&val, 1);
  return val;
}

void OutStream::writeFloat(float f) {
  writeData(&f, sizeof(float));
}
float InStream::readFloat() {
  float val;
  readData(&val, sizeof(float));
  return val;
}

void OutStream::writeDouble(double d) {
  writeData(&d, sizeof(double));
}
double InStream::readDouble() {
  double val;
  readData(&val, sizeof(double));
  return val;
}

}