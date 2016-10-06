#ifndef DIGGLER_CRYPTO_SHA256_HPP
#define DIGGLER_CRYPTO_SHA256_HPP

#include <sodium.h>

#include "CryptoData.hpp"

namespace Diggler {
namespace Crypto {

struct SHA256 {
  constexpr static size_t DigestBytes = crypto_hash_sha256_BYTES;

  struct Digest : CryptoData<SHA256::DigestBytes> {
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
  };

  crypto_hash_sha256_state state;

  SHA256() {
    crypto_hash_sha256_init(&state);
  }

  template<typename T>
  void update(const T *data, size_t len) {
    crypto_hash_sha256_update(&state, data, len);
  }

  void finalize(unsigned char *digest) {
    crypto_hash_sha256_final(&state, digest);
  }

  Digest finalize() {
    Digest digest;
    crypto_hash_sha256_final(&state, digest.bytes);
    return digest;
  }
};

}
}

#endif /* DIGGLER_CRYPTO_SHA256_HPP */
