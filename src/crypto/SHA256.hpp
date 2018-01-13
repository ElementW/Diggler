#ifndef DIGGLER_CRYPTO_SHA256_HPP
#define DIGGLER_CRYPTO_SHA256_HPP

#include <sodium.h>

#include "CryptoData.hpp"

namespace diggler {
namespace Crypto {

struct SHA256 {
  constexpr static size_t DigestBytes = crypto_hash_sha256_BYTES;

  struct Digest : CryptoData<SHA256::DigestBytes> {
    using CryptoData<SHA256::DigestBytes>::CryptoData;
    using CryptoData<SHA256::DigestBytes>::operator=;

    /* Non-constant-time comparison */
    bool operator==(const Digest &o) const {
      return memcmp(data, o.data, Length) == 0;
    }

    bool operator!=(const Digest &o) const {
      return !operator==(o);
    }

    bool operator>(const Digest &o) const {
      return memcmp(data, o.data, Length) > 0;
    }

    bool operator>=(const Digest &o) const {
      return memcmp(data, o.data, Length) >= 0;
    }

    bool operator<(const Digest &o) const {
      return memcmp(data, o.data, Length) < 0;
    }

    bool operator<=(const Digest &o) const {
      return memcmp(data, o.data, Length) <= 0;
    }

    /* Allow implicit string casting */
    using CryptoData<SHA256::DigestBytes>::operator std::string;
  };

  crypto_hash_sha256_state state;

  SHA256() {
    crypto_hash_sha256_init(&state);
  }

  template<typename T>
  void update(const T *data, size_t len) {
    crypto_hash_sha256_update(&state,
      reinterpret_cast<const unsigned char*>(data), len * sizeof(T));
  }

  template<size_t B>
  void update(const CryptoData<B> &data) {
    crypto_hash_sha256_update(&state, data.bytes, B);
  }

  void finalize(unsigned char *digest) {
    crypto_hash_sha256_final(&state, digest);
  }

  Digest finalize() {
    Digest digest;
    crypto_hash_sha256_final(&state, digest.bytes);
    return digest;
  }

  template<typename T>
  static Digest hash(const T *data, size_t len) {
    Digest digest;
    crypto_hash_sha256(digest.bytes,
      reinterpret_cast<const unsigned char*>(data), len * sizeof(T));
    return digest;
  }
};

}
}

#endif /* DIGGLER_CRYPTO_SHA256_HPP */
