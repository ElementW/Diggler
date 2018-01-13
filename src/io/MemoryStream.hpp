#ifndef DIGGLER_MEMORY_STREAM_HPP
#define DIGGLER_MEMORY_STREAM_HPP

#include "Stream.hpp"

namespace diggler {
namespace io {

class MemoryStream : public virtual SeekableStream {
protected:
  byte *m_data;
  SizeT m_length;
  PosT m_cursor;

  MemoryStream(void*, SizeT);
  MemoryStream() : MemoryStream(nullptr, 0) {}

public:
  virtual PosT tell() const override {
    return m_cursor;
  }
  virtual void seek(OffT, Whence = Set) override;
  inline void seek(PosT pos) {
    seek(static_cast<OffT>(pos), Whence::Set);
  }

  virtual SizeT length() const {
    return m_length;
  }
  virtual const void* data() const {
    return m_data;
  }
};

class InMemoryStream : public virtual MemoryStream, public SizedStream, public virtual InStream {
protected:
  InMemoryStream() {}

public:
  InMemoryStream(const void *data, SizeT len);
  virtual ~InMemoryStream() {}

  SizeT length() const override;

  virtual void readData(void *data, SizeT len) override;
};

class OutMemoryStream : public virtual MemoryStream, public virtual OutStream {
protected:
  SizeT m_allocated;
  virtual void fit(SizeT);

public:
  OutMemoryStream(SizeT prealloc = 0);
  virtual ~OutMemoryStream();

  virtual bool eos() const override {
    return false;
  }

  virtual void writeData(const void *data, SizeT len) override;
};

}
}

#endif /* DIGGLER_MEMORY_STREAM_HPP */
