#ifndef DIGGLER_CRYPTO_SIGN_HPP
#define DIGGLER_CRYPTO_SIGN_HPP

#include <sodium.h>

#include "CryptoData.hpp"

namespace Diggler {
namespace Crypto {
namespace Sign {

constexpr size_t PublicKeyBytes = crypto_sign_PUBLICKEYBYTES;
constexpr size_t SecretKeyBytes = crypto_sign_SECRETKEYBYTES;
constexpr size_t SignatureBytes = crypto_sign_BYTES;

struct PublicKey : CryptoData<PublicKeyBytes> {
};

struct SecretKey : MlockedCryptoData<SecretKeyBytes> {
};

struct Signature : CryptoData<SignatureBytes> {
};

void keypair(PublicKey &pk, SecretKey &sk) {
  crypto_sign_keypair(pk.bytes, sk.bytes);
}

void sign(const unsigned char *m, unsigned long long mlen, const SecretKey &sk, Signature &sig) {
  crypto_sign_detached(sig.bytes, nullptr, m, mlen, sk.bytes);
}

bool verify(const unsigned char *m, unsigned long long mlen, Signature &sig, const PublicKey &pk) {
  return crypto_sign_verify_detached(sig.bytes, m, mlen, pk.bytes);
}

}
}
}

#endif /* DIGGLER_CRYPTO_SIGN_HPP */
