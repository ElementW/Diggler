#include "FastRand.hpp"

#include <random>

namespace diggler {

uint FastRandInit() {
  static std::random_device rd;
  static std::mt19937 mt(rd());
  return mt();
}

}
