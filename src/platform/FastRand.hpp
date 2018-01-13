#ifndef DIGGLER_PLATFORM_FAST_RAND_HPP
#define DIGGLER_PLATFORM_FAST_RAND_HPP

#include "Types.hpp"

namespace diggler {

uint FastRandInit();
static thread_local uint FastRand_Seed = FastRandInit();
#define FastRandSeed(x) FastRand_Seed=x;

/** Fast pseudo-random number generator, very inaccurate
 * @returns A random integer in range [0, 2^31-1]
 */
inline int FastRand() {
  FastRand_Seed = (514229*((FastRand_Seed+4631018)>>1))^0x51d75169;
  return FastRand_Seed & 0x7FFFFFFF;
}
/** Fast pseudo-random number generator, very inaccurate
 * @returns A random integer in range [0, max]
 */
inline int FastRand(int max) {
  return FastRand() % (max+1);
}
/** Fast pseudo-random number generator, very inaccurate
 * @returns A random integer in range [min, max]
 */
inline int FastRand(int min, int max) {
  return min + (FastRand() % (max-min+1) );
}
/** Fast pseudo-random number generator, very inaccurate
 * @returns A random float in range [0.0, 1.0]
 */
inline float FastRandF() {
  return static_cast<float>(FastRand()) / 0x7FFFFFFF;
}

}

#endif /* DIGGLER_PLATFORM_FAST_RAND_HPP */
