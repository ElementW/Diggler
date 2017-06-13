#ifndef DIGGLER_UTIL_MEMORY_TRACKER_HPP
#define DIGGLER_UTIL_MEMORY_TRACKER_HPP

#include <cstdint>
#include <string>
#include <vector>

namespace Diggler {
namespace Util {
namespace MemoryTracker {

void init();

#ifdef DIGGLER_ENABLE_MEMORY_TRACKER
// DJB2 hash
// http://www.cse.yorku.ca/~oz/hash.html
constexpr uint_fast32_t hash(const unsigned char *str) {
  uint_fast32_t hash = 5381;
  unsigned char c = 0;
  while ((c = *str++)) {
      hash = ((hash << 5) + hash) + c;
  }
  return hash;
}
constexpr uint_fast32_t hash(const char *str) {
  uint_fast32_t hash = 5381;
  unsigned char c = 0;
  while ((c = static_cast<unsigned char>(*str++))) {
      hash = ((hash << 5) + hash) + c;
  }
  return hash;
}
#endif /* DIGGLER_ENABLE_MEMORY_TRACKER */

// Thread-local
void setCategory(const char *cat, uint_fast32_t catHash);

// Thread-local
inline void setCategory(const char *cat) {
#ifdef DIGGLER_ENABLE_MEMORY_TRACKER
  if (cat == nullptr) {
    setCategory(nullptr, 0);
  } else {
    setCategory(cat, hash(cat));
  }
#else
  (void) cat;
#endif /* DIGGLER_ENABLE_MEMORY_TRACKER */
}


struct CategoryStats {
  std::string name;
  uint_fast32_t bytesUsed;
};

std::vector<CategoryStats> categoryStats();

class ScopedCategory final {
#ifdef DIGGLER_ENABLE_MEMORY_TRACKER
private:
  int_fast16_t m_lastCat;
  static int_fast16_t currentCat();
  static void setCat(int_fast16_t);

public:
  ScopedCategory(const char *cat) {
    m_lastCat = currentCat();
    setCategory(cat);
  }
  ~ScopedCategory() {
    setCat(m_lastCat);
  }
#else
public:
  ScopedCategory(const char*) {}
#endif /* DIGGLER_ENABLE_MEMORY_TRACKER */
};

}
}
}

#endif /* DIGGLER_UTIL_MEMORY_TRACKER_HPP */
