#ifndef DIGGLER_IO_FILE_STREAM_HPP
#define DIGGLER_IO_FILE_STREAM_HPP

#include <fstream>

#include "Stream.hpp"

namespace Diggler {
namespace IO {

class InFileStream : public InSeekableStream, public SizedStream {
protected:
  mutable std::ifstream strm;

public:
  InFileStream(const std::string &path);
  ~InFileStream();

  SizeT length() const override;

  void readData(void *data, SizeT len) override;
  PosT tell() const override;
  void seek(OffT, Whence = Set) override;
};

class OutFileStream : public OutSeekableStream {
protected:
  mutable std::ofstream strm;

public:
  OutFileStream(const std::string &path);
  ~OutFileStream();

  virtual bool eos() const override {
    return false;
  }

  void writeData(const void *data, SizeT len) override;
  PosT tell() const override;
  void seek(OffT, Whence = Set) override;
};

}
}

#endif /* DIGGLER_IO_FILE_STREAM_HPP */
