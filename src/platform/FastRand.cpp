#include "FastRand.hpp"

#include <random>

namespace Diggler {

uint FastRandInit() {
  static std::random_device rd;
  static std::mt19937 mt(rd());
  return mt();
}

}
