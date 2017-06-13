#include "MemoryTracker.hpp"

#include <algorithm> // std::max
#include <array>
#include <atomic>
#include <cstddef>
#include <cstdlib> // posix_memalign
#include <cstring>
#include <map>
#include <string>

#include "Jemalloc.hpp" // Must be included after cstdlib

#include <read_write_mutex.hpp>

using CategoryIndex = int_fast16_t;
static constexpr CategoryIndex MaxCategories = 512;

static std::array<std::atomic_uintptr_t, MaxCategories> Categories;

static emilib::FastReadWriteMutex CategoryMappingMutex;
struct CategoryMapping {
  std::string name;
  CategoryIndex index;
};
static std::map<uint_fast32_t, CategoryMapping> CategoryMappings;
static CategoryIndex LastIndex = 0;

static thread_local CategoryIndex CurrentCategory = 0;

extern "C" {

void* __real_malloc(size_t size);
void* __wrap_malloc(size_t size) {
  if (size == 0) {
    return nullptr;
  }
  void *ptr = __real_malloc(size);
  if (ptr != nullptr) {
    Categories[CurrentCategory] += static_cast<uintptr_t>(sallocx(ptr, 0));
  }
  return ptr;
}

void* __real_calloc(size_t nmemb, size_t size);
void* __wrap_calloc(size_t nmemb, size_t size) {
  if (size == 0 || nmemb == 0) {
    return nullptr;
  }
  void *ptr = __real_calloc(nmemb, size);
  if (ptr != nullptr) {
    Categories[CurrentCategory] += static_cast<uintptr_t>(sallocx(ptr, 0));
  }
  return ptr;
}

void* __real_realloc(void *ptr, size_t size);
void* __wrap_realloc(void *ptr, size_t size) {
  if (ptr == nullptr) {
    return malloc(size);
  }
  const uintptr_t oldSize = static_cast<uintptr_t>(sallocx(ptr, 0));
  ptr = __real_realloc(ptr, size);
  if (ptr != nullptr) {
    Categories[CurrentCategory] -= oldSize;
    Categories[CurrentCategory] += static_cast<uintptr_t>(sallocx(ptr, 0));
  }
  return ptr;
}

void __real_free(void *ptr);
void __wrap_free(void *ptr) {
  if (ptr == nullptr) {
    return;
  }
  Categories[CurrentCategory] -= static_cast<uintptr_t>(sallocx(ptr, 0));
  __real_free(ptr);
}

int __real_posix_memalign(void **memptr, size_t alignment, size_t size);
int __wrap_posix_memalign(void **memptr, size_t alignment, size_t size) {
  int res = __real_posix_memalign(memptr, alignment, size);
  if (res == 0) {
    Categories[CurrentCategory] += static_cast<uintptr_t>(sallocx(*memptr, 0));
  }
  return res;
}

}

void* operator new(std::size_t count) {
  void *ptr = malloc(count);
  if (!ptr) {
    throw std::bad_alloc();
  }
  return ptr;
}

void* operator new(std::size_t count, const std::nothrow_t& tag) noexcept {
  return malloc(count);
}

// 3.7.3.1/2:
// The effect of dereferencing a pointer returned as a request for zero size is undefined.
void* operator new[](std::size_t count) {
  void *ptr = malloc(count == 0 ? 1 : count);
  if (!ptr) {
    throw std::bad_alloc();
  }
  return ptr;
}

void* operator new[](std::size_t count, const std::nothrow_t& tag) noexcept {
  return malloc(count == 0 ? 1 : count);
}

void operator delete(void* ptr) {
  free(ptr);
}

void operator delete(void* ptr, const std::nothrow_t& tag) noexcept {
  free(ptr);
}

void operator delete[](void* ptr) {
  free(ptr);
}

void operator delete[](void* ptr, const std::nothrow_t& tag) noexcept {
  free(ptr);
}


namespace Diggler {
namespace Util {
namespace MemoryTracker {

void init() {
}

void setCategory(const char *cat, uint_fast32_t catHash) {
  emilib::ReadLock<emilib::FastReadWriteMutex> readLock(CategoryMappingMutex);
  if (cat == nullptr) {
    CurrentCategory = 0;
  } else {
    auto it = CategoryMappings.find(catHash);
    if (it == CategoryMappings.end()) {
      readLock.unlock();
      emilib::WriteLock<emilib::FastReadWriteMutex> writeLock(CategoryMappingMutex);
      it = CategoryMappings.emplace(std::piecewise_construct,
          std::forward_as_tuple(catHash),
          std::forward_as_tuple(CategoryMapping { cat, ++LastIndex })).first;
      writeLock.unlock();
      readLock.lock();
    }
    CurrentCategory = it->second.index;
  }
}

std::vector<CategoryStats> categoryStats() {
  emilib::ReadLock<emilib::FastReadWriteMutex> readLock(CategoryMappingMutex);
  std::vector<CategoryStats> stats(CategoryMappings.size() + 1);
  stats.emplace_back(CategoryStats { "Unknown", Categories[0].load() });
  for (auto cit = CategoryMappings.cbegin(); cit != CategoryMappings.cend(); ++cit) {
    stats.emplace_back(CategoryStats { cit->second.name, Categories[cit->second.index].load() });
  }
  return stats;
}

int_fast16_t ScopedCategory::currentCat() {
  return CurrentCategory;
}

void ScopedCategory::setCat(int_fast16_t cat) {
  CurrentCategory = cat;
}

}
}
}
