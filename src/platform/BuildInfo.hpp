#ifndef DIGGLER_PLATFORM_BUILDINFO_HPP
#define DIGGLER_PLATFORM_BUILDINFO_HPP

#define BUILDINFO_TIME __TIME__
#define BUILDINFO_DATE __DATE__

#if defined(WINDOWS) || defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64) // Windows
  #define BUILDINFO_PLATFORM "Windows"
  #define BUILDINFO_PLATFORM_WINDOWS
  #if defined(__CYGWIN__)
    #define BUILDINFO_PLATFORM_CYGWIN
  #endif
#elif defined(__ANDROID_API__) // Android
  #define BUILDINFO_PLATFORM "Android"
  #define BUILDINFO_PLATFORM_ANDROID
  #define BUILDINFO_PLATFORM_PTHREAD
  #define BUILDINFO_PLATFORM_MMAP
  #define BUILDINFO_PLATFORM_UNIXLIKE
#elif defined(__linux__) || defined(linux) || defined(__linux) // Linux
  #define BUILDINFO_PLATFORM "Linux"
  #define BUILDINFO_PLATFORM_LINUX
  #define BUILDINFO_PLATFORM_PTHREAD
  #define BUILDINFO_PLATFORM_MMAP
  #define BUILDINFO_PLATFORM_UNIXLIKE
#elif defined(__FreeBSD__) || defined(__NetBSD__) || defined(__OpenBSD__) || \
      defined(__bsdi__) || defined(__DragonFly__) || defined(BSD) // BSD
  #define BUILDINFO_PLATFORM "BSD"
  #define BUILDINFO_PLATFORM_BSD
  #define BUILDINFO_PLATFORM_PTHREAD
  #define BUILDINFO_PLATFORM_MMAP
  #define BUILDINFO_PLATFORM_UNIXLIKE
  #ifdef __FreeBSD__
    #define BUILDINFO_PLATFORM "FreeBSD"
    #define BUILDINFO_PLATFORM_FREEBSD
  #endif
  #ifdef __NetBSD__
    #define BUILDINFO_PLATFORM "NetBSD"
    #define BUILDINFO_PLATFORM_NETBSD
  #endif
  #ifdef __OpenBSD__
    #define BUILDINFO_PLATFORM "OpenBSD"
    #define BUILDINFO_PLATFORM_OPENBSD
  #endif
  #ifdef __DragonflyBSD__
    #define BUILDINFO_PLATFORM "DragonflyBSD"
    #define BUILDINFO_PLATFORM_DRAGONFLYBSD
  #endif
#elif (defined(__APPLE__) && defined(__MACH__)) // Mac
  #define BUILDINFO_PLATFORM "macOS"
  #define BUILDINFO_PLATFORM_MACOS
  #define BUILDINFO_PLATFORM_MMAP
  #define BUILDINFO_PLATFORM_UNIXLIKE
#else // Any other
  #define BUILDINFO_PLATFORM "Unknown"
  #define BUILDINFO_PLATFORM_UNKNOWN
#endif

#if defined(__x86_64__) || defined(__ppc64__) || defined(_WIN64)
  #define HAS_NATIVE_64BIT 1
#endif

#endif /* DIGGLER_PLATFORM_BUILDINFO_HPP */
