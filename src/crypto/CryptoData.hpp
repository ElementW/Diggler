#ifndef DIGGLER_CRYPTO_CRYPTODATA_HPP
#define DIGGLER_CRYPTO_CRYPTODATA_HPP

#include <cstring>
#include <stdexcept>
#include <string>
#include <vector>

#include <sodium.h>

namespace Diggler {
namespace Crypto {

template<size_t B>
struct CryptoData {
  constexpr static size_t Length = B;

  union {
    unsigned char data[Length], bytes[Length];
  };

  CryptoData() = default;
  CryptoData(const CryptoData&) = default;
  CryptoData(CryptoData&&) = default;
  CryptoData(const std::string &s) {
    if (s.length() != Length) {
      throw std::runtime_error("Invalid string length");
    }
    memcpy(data, s.data(), Length);
  }
  CryptoData(const std::vector<char> &v) {
    if (v.size() != Length) {
      throw std::runtime_error("Invalid vector length");
    }
    memcpy(data, v.data(), Length);
  }
  CryptoData(const std::vector<unsigned char> &v) {
    if (v.size() != Length) {
      throw std::runtime_error("Invalid vector length");
    }
    memcpy(data, v.data(), Length);
  }

  CryptoData& operator=(const CryptoData&) = default;
  CryptoData& operator=(CryptoData&&) = default;

  CryptoData& operator=(const std::string &s) {
    if (s.length() != Length) {
      throw std::runtime_error("Invalid string length");
    }
    memcpy(data, s.data(), Length);
    return *this;
  }
  CryptoData& operator=(const std::vector<char> &v) {
    if (v.size() != Length) {
      throw std::runtime_error("Invalid vector length");
    }
    memcpy(data, v.data(), Length);
    return *this;
  }
  CryptoData& operator=(const std::vector<unsigned char> &v) {
    if (v.size() != Length) {
      throw std::runtime_error("Invalid vector length");
    }
    memcpy(data, v.data(), Length);
    return *this;
  }

  explicit operator std::string() const {
    return std::string(reinterpret_cast<const char*>(data), Length);
  }
  explicit operator std::vector<char>() const {
    std::vector<char> ret(Length);
    memcpy(ret.data(), data, Length);
    return ret;
  }
  explicit operator std::vector<unsigned char>() const {
    std::vector<unsigned char> ret(Length);
    memcpy(ret.data(), data, Length);
    return ret;
  }

  bool operator==(const CryptoData<B> &o) const {
    return sodium_memcmp(data, o.data, Length) == 0;
  }

  bool operator!=(const CryptoData<B> &o) const {
    return !operator==(o);
  }

  bool operator==(const std::string &o) const {
    return o.length() == Length and sodium_memcmp(data, o.data(), Length) == 0;
  }
  bool operator==(const std::vector<char> &o) const {
    return o.size() == Length and sodium_memcmp(data, o.data(), Length) == 0;
  }
  bool operator==(const std::vector<unsigned char> &o) const {
    return o.size() == Length and sodium_memcmp(data, o.data(), Length) == 0;
  }

  bool operator!=(const std::string &o) const {
    return !operator==(o);
  }
  bool operator!=(const std::vector<char> &o) const {
    return !operator==(o);
  }
  bool operator!=(const std::vector<unsigned char> &o) const {
    return !operator==(o);
  }

  unsigned char& operator[](size_t idx) {
    return data[idx];
  }

  unsigned char operator[](size_t idx) const {
    return data[idx];
  }

  void zerofill() {
    sodium_memzero(data, Length);
  }

  std::string hex(bool caps = false) const {
    std::string hex(Length * 2, '\0');
    const char *hextab = caps ? "0123456789ABCDEF" : "0123456789abcdef";
    for (size_t i = 0; i < Length; ++i) {
      hex[i * 2 + 0] = hextab[data[i] >> 4];
      hex[i * 2 + 1] = hextab[data[i] & 0xF];
    }
    return hex;
  }
};

template<size_t B>
struct MlockedCryptoData : CryptoData<B> {
  MlockedCryptoData() {
    sodium_mlock(this->data, this->Length);
  }

  MlockedCryptoData(MlockedCryptoData<B> &o) {
    sodium_mlock(this->data, this->Length);
    memcpy(this->data, o.data, this->Length);
  }

  ~MlockedCryptoData() {
    sodium_munlock(this->data, this->Length);
  }
};

}
}

#endif /* DIGGLER_CRYPTO_CRYPTODATA_HPP */
