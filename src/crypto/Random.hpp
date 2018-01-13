#ifndef DIGGLER_CRYPTO_RANDOM_HPP
#define DIGGLER_CRYPTO_RANDOM_HPP

#include <sodium.h>

#include "CryptoData.hpp"
#include "../platform/Types.hpp"

namespace diggler {
namespace Crypto {
namespace Random {

uint32 random() {
  return randombytes_random();
}

uint32 uniform(const uint32 upperBound) {
  return randombytes_uniform(upperBound);
}

void buf(void *const buf, const size_t size) {
  randombytes_buf(buf, size);
}

template<size_t B>
void randomData(CryptoData<B> &cd) {
  randombytes_buf(cd.bytes, cd.Length);
}

void stir() {
  randombytes_stir();
}

}
}
}

#endif /* DIGGLER_CRYPTO_RANDOM_HPP */
