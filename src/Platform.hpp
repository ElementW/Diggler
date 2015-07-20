#ifndef PLATFORM_HPP
#define PLATFORM_HPP

#define BUILDINFO_TIME __TIME__
#define BUILDINFO_DATE __DATE__

#if defined(WINDOWS) || defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64) // Windows
	#define BUILDINFO_PLATFORM "Windows"
	#define BUILDINFO_PLATFORM_WINDOWS
#elif defined(__ANDROID_API__) // Android
	#define BUILDINFO_PLATFORM "Android"
	#define BUILDINFO_PLATFORM_ANDROID
	#define BUILDINFO_PLATFORM_PTHREAD
#elif defined(__linux__) || defined(linux) || defined(_linux) // Linux
	#define BUILDINFO_PLATFORM "Linux"
	#define BUILDINFO_PLATFORM_LINUX
	#define BUILDINFO_PLATFORM_PTHREAD
#elif defined(__APPLE__) // Mac
	#define BUILDINFO_PLATFORM "Mac"
	#define BUILDINFO_PLATFORM_MAC
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

namespace Diggler {

typedef int32_t int32;
typedef uint32_t uint;
typedef uint32_t uint32;
typedef uint64_t uint64;
typedef uint16_t uint16;
typedef uint8_t uint8;
typedef int64_t int64;
typedef int16_t int16;
typedef int8_t int8;
typedef uint8_t byte;
typedef char32_t char32;
typedef char16_t char16;

typedef std::string String;

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


extern uint FastRand_Seed;
#define FastRandSeed(x) FastRand_Seed=x;
///
/// Fast pseudo-random number generator, very inaccurate
/// @returns A random integer in range [0, 2^31-1]
///
inline int FastRand() {
	FastRand_Seed = (514229*((FastRand_Seed+4631018)>>1))^0x51d75169;
	return FastRand_Seed & 0x7FFFFFFF;
}
///
/// Fast pseudo-random number generator, very inaccurate
/// @returns A random integer in range [0, max]
///
inline int FastRand(int max) {
	return FastRand() % (max+1);
}
///
/// Fast pseudo-random number generator, very inaccurate
/// @returns A random integer in range [min, max]
///
inline int FastRand(int min, int max) {
	return min + (FastRand() % (max-min+1) );
}
///
/// Fast pseudo-random number generator, very inaccurate
/// @returns A random float in range [0.0, 1.0[
///
inline float FastRandF() {
	return (float)FastRand() / 0x7FFFFFFF;
}

///
/// Real Modulus
/// @returns Real modulus operation result, as such mod(x,y) is always positive
///
int rmod(int x, int y);
float rmod(float x, float y);
double rmod(double x, double y);

///
/// Divide rounding down / Modulo quotient
/// @returns x/y rounded down / Q in modulus' A=B*Q+R equation
///
/*[[gnu::always_inline]]*/ inline int divrd(int x, uint y) {
	return x/(int)y - (x < 0 ? 1 : 0);
}

///
/// @return Floored value of f, as an integer
/// @see ::std::floor For results as float or double
///
/*[[gnu::always_inline]]*/ inline int floor(const float f) {
	if (f >= 0)
		return (int)f;
	return ((int)f)-1;
}

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