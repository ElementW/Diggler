#ifndef DIGGLER_PLATFORM_FIXES
#define DIGGLER_PLATFORM_FIXES

#ifdef _GNU_SOURCE
  #include "Fixes.glibc.hpp"
#endif

#ifdef _WIN32
  #include "Fixes.windows.hpp"
#endif

#endif /* DIGGLER_PLATFORM_FIXES */
