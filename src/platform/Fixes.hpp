#ifndef DIGGLER_PLATFORM_FIXES
#define DIGGLER_PLATFORM_FIXES

#if defined(__linux__)
  // No way to detect glibc without importing one of its headers,
  // assume glibc by default on Linux. :(
  #include "Fixes.glibc.hpp"
#elif defined(_WIN32)
  #include "Fixes.windows.hpp"
#endif

#endif /* DIGGLER_PLATFORM_FIXES */
