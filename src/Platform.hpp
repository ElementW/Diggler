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

namespace Diggler {

using int32  = int32_t;
using uint   = uint32_t;
using uint32 = uint32_t;
using uint64 = uint64_t;
using uint16 = uint16_t;
using uint8  = uint8_t;
using int64  = int64_t;
using int16  = int16_t;
using int8   = int8_t;
using byte   = uint8_t;
using char32 = char32_t;
using char16 = char16_t;

using String = std::string;

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
constexpr int rmod(int x, int y) {
	const int ret = x % y;
	return (ret < 0) ? y+ret : ret;
}
float rmod(float x, float y);
double rmod(double x, double y);

///
/// Divide rounding down / Modulo quotient
/// @returns x/y rounded down / Q in modulus' A=B×Q+R equation
///
constexpr inline int divrd(int x, uint y) {
	return (x < 0) ? (x+1)/(int)y-1 : x/(int)y;
}

///
/// @returns Floored value of f, as an integer
/// @see ::std::floor For results as float or double
///
constexpr inline int floor(const float f) {
	return (f > 0) ? (int)f : ((int)f)-1;
}

///
/// @returns Ceiling value of f, as an integer
/// @see ::std::ceil For results as float or double
///
constexpr inline int ceil(const float f) {
	return (f < 0) ? (int)f : ((int)f)+1;
}

///
/// @returns 1×[sign of v]. 0 if v == 0.
///
constexpr inline int signum(float v) {
	return (v > 0) ? 1 : (v < 0) ? -1 : 0;
}

///
/// @returns the smallest positive t such that s+t*ds is an integer.
/// @see http://gamedev.stackexchange.com/questions/47362/cast-ray-to-select-block-in-voxel-game
///
constexpr float intbound(float s, float ds) {
	return (ds < 0) ? intbound(-s, -ds) : (1-rmod(s, 1.f))/ds;
}


using FourCC = uint32;
constexpr inline FourCC MakeFourCC(char a, char b, char c, char d) {
#if defined(BUILDINFO_LITTLE_ENDIAN)
	return d | c >> 8 | b >> 16 | a >> 24;
#else
	return a | b >> 8 | c >> 16 | d >> 24;
#endif
}
constexpr inline FourCC MakeFourCC(const char *s) {
	return MakeFourCC(s[0], s[1], s[2], s[3]);
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
