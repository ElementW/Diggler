#ifndef DIGGLER_CRYPTO_DIFFIE_HELLMAN_HPP
#define DIGGLER_CRYPTO_DIFFIE_HELLMAN_HPP

#include <sodium.h>

#include "CryptoData.hpp"

namespace diggler {
namespace Crypto {
namespace DiffieHellman {

constexpr static size_t ScalarBytes = crypto_scalarmult_SCALARBYTES;
constexpr static size_t Bytes = crypto_scalarmult_BYTES;

struct SecretKey : MlockedCryptoData<ScalarBytes> {
};

struct PublicKey : CryptoData<Bytes> {
  using CryptoData<Bytes>::CryptoData;
  using CryptoData<Bytes>::operator=;
};

struct SharedSecret : MlockedCryptoData<Bytes> {
};

inline void scalarmultBase(const SecretKey &sk, PublicKey &pk) {
  crypto_scalarmult_base(pk.bytes, sk.bytes);
}

[[gnu::warn_unused_result]]
inline bool scalarmult(const SecretKey &sk, const PublicKey &pk, SharedSecret &ss) {
  return crypto_scalarmult(ss.bytes, sk.bytes, pk.bytes);
}

}
}
}

#endif /* DIGGLER_CRYPTO_DIFFIE_HELLMAN_HPP */
