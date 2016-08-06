#ifndef PLATFORM_HPP
#define PLATFORM_HPP

#define BUILDINFO_TIME __TIME__
#define BUILDINFO_DATE __DATE__

#if defined(__LITTLE_ENDIAN__)
  #define BUILDINFO_LITTLE_ENDIAN
#else
  #define BUILDINFO_BIG_ENDIAN
#endif

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
#elif (defined(__APPLE__) && defined(__MACH__)) // Mac
  #define BUILDINFO_PLATFORM "Mac"
  #define BUILDINFO_PLATFORM_MAC
  #define BUILDINFO_PLATFORM_MMAP
  #define BUILDINFO_PLATFORM_UNIXLIKE
#else // Any other
  #define BUILDINFO_PLATFORM "Unknown"
  #define BUILDINFO_PLATFORM_UNKNOWN
#endif

#if __x86_64__ || __ppc64__ || _WIN64
  #define HAS_NATIVE_64BIT 1
#endif

#include <vector>
#include <string>
#include <iostream>

#include "platform/FastRand.hpp"
#include "platform/FourCC.hpp"
#include "platform/Math.hpp"
#include "platform/Types.hpp"

namespace Diggler {

namespace proc {
  /// @returns The executable's absolute path
  std::string getExecutablePath();

  /// @returns The executable's absolute path directory, including the end slash (/)
  std::string getExecutableDirectory();
}

/// @returns The system's error output stream
std::ostream& getErrorStreamRaw();
#ifdef IN_IDE_PARSER
std::ostream& getErrorStream();
#else
#define getErrorStream() getErrorStreamRaw() << __FILENAME__ << ':' << __LINE__ << ' '
#endif

/// @returns The system's debug output stream
std::ostream& getDebugStreamRaw();
#ifdef IN_IDE_PARSER
std::ostream& getDebugStream();
#else
#define getDebugStream() getDebugStreamRaw() << __FILENAME__ << ':' << __LINE__ << ' '
#endif

/// @returns The system's output stream
std::ostream& getOutputStreamRaw();
#ifdef IN_IDE_PARSER
std::ostream& getOutputStream();
#else
#define getOutputStream() getOutputStreamRaw() << __FILENAME__ << ':' << __LINE__ << ' '
#endif

extern const char *UserdataDirsName;

std::string getConfigDirectory();
std::string getCacheDirectory();


/// @returns The absolute assets directory path
std::string getAssetsDirectory(const std::string &type);

/// @returns The absolute assets directory path
std::string getAssetsDirectory();

/// @returns The absolute asset path
std::string getAssetPath(const std::string &name);

/// @returns The absolute asset path
std::string getAssetPath(const std::string &type, const std::string &name);


namespace fs {

///
/// @returns `true` if specified `path` is a directory, `false` otherwise
///
bool isDir(const std::string &path);

///
/// @brief Concatenates two path parts
/// Concatenates two paths avoiding path element separator duplication.
/// @returns Concatenated path
///
std::string pathCat(const std::string& first, const std::string &second);

///
/// @brief Enumerates a given directory `path`'s content
/// @returns Vector of each element's name
///
std::vector<std::string> getContents(const std::string &path);

///
/// @brief Enumerates a given directory `path`'s subdirectories
/// @returns Vector of each subdirectory's name
///
std::vector<std::string> getDirs(const std::string &path);

///
/// @brief Enumerates a given directory `path`'s files
/// @returns Vector of each file's name
///
std::vector<std::string> getFiles(const std::string &path);

///
/// @brief Reads a whole file denoted by `path` and returns its content
/// Only works if element filesystem entry `path` is a normal file.
/// Directories, FIFOs (pipes), block/character devices files aren't supported and
/// will return an empty string.
/// Softlinks are followed.
/// @returns `path`'s content, or an empty string if `path` isn't a normal file.
///
std::string readFile(const std::string &path);

///
/// Get `path`'s hierarchical parent in the filesystem.
/// @note No symlinks are resolved in the operation, and getting the parent of the
///       topmost directory (root) will return itself as its parent.
///       Also, parent directories may be unreadable due to filesystem permissions.
/// @return `path`'s filesystem parent path
///
std::string getParent(const std::string &path);
}

}

#endif
