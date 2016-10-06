#ifndef DIGGLER_CRYPTO_CRYPTODATA_HPP
#define DIGGLER_CRYPTO_CRYPTODATA_HPP

#include <cstring>
#include <string>

#include <sodium.h>

namespace Diggler {
namespace Crypto {

template<size_t B>
struct CryptoData {
  constexpr static size_t Length = B;

  union {
    unsigned char data[Length], bytes[Length];
  };

  bool operator==(const CryptoData<B> &o) const {
    return sodium_memcmp(data, o.data, Length) == 0;
  }

  bool operator!=(const CryptoData<B> &o) const {
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
