#ifndef PLATFORM_HPP
#define PLATFORM_HPP

#define BUILDINFO_TIME __TIME__
#define BUILDINFO_DATE __DATE__

#if defined(WINDOWS) || defined(WIN32) || defined(_WIN32) || defined(WIN64) || defined(_WIN64) // Windows
	#define BUILDINFO_PLATFORM "Windows"
	#define BUILDINFO_PLATFORM_WINDOWS
#elif defined(__ANDROID_API__)
	#define BUILDINFO_PLATFORM "Android"
	#define BUILDINFO_PLATFORM_ANDROID
#elif defined(__linux__) || defined(linux) || defined(_linux) // Linux
	#define BUILDINFO_PLATFORM "Linux"
	#define BUILDINFO_PLATFORM_LINUX
#elif defined(__APPLE__) || defined(__MACH__) // Mac
	#define BUILDINFO_PLATFORM "Mac"
	#define BUILDINFO_PLATFORM_MAC
#else // Any other
	#define BUILDINFO_PLATFORM "Unknown"
#endif

#if _WIN64 || __x86_64__ || __ppc64__
	#define HAS_NATIVE_64BIT 1
#endif

#include <vector>
#include <string>
#include <iostream>

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

namespace Diggler {

extern double (*getTime)();

/// @returns The executable's absolute path
std::string getExecutablePath();

/// @returns The executable's absolute path directory, including the end slash (/)
std::string getExecutableDirectory();

#ifdef IN_IDE_PARSER
/// @returns The system's error output stream
std::ostream& getErrorStream();
#else
std::ostream& getErrorStreamImpl();
#define getErrorStream() getErrorStreamImpl() << __FILENAME__ << ':' << __LINE__ << ' '
#endif

#ifdef IN_IDE_PARSER
/// @returns The system's debug output stream
std::ostream& getDebugStream();
#else
std::ostream& getDebugStreamImpl();
#define getDebugStream() getDebugStreamImpl() << __FILENAME__ << ':' << __LINE__ << ' '
#endif

#ifdef IN_IDE_PARSER
/// @returns The system's output stream
std::ostream& getOutputStream();
#else
std::ostream& getOutputStreamImpl();
#define getOutputStream() getOutputStreamImpl() << __FILENAME__ << ':' << __LINE__ << ' '
#endif


/// @returns The absolute specific assets directory path, including the end slash (/)
std::string getAssetsDirectory(const std::string &type);

/// @returns The absolute assets directory path, including the end slash (/)
std::string getAssetsDirectory();

/// @returns The absolute asset path
std::string getAssetPath(const std::string &name);

/// @returns The absolute asset path
std::string getAssetPath(const std::string &type, const std::string &name);


extern uint FastRand_Seed;
#define FastRandSeed(x) FastRand_Seed=x;
///
/// Fast pseudo-random number generator, very inaccurate
/// @returns a random number between 0 and 2^31
///
inline int FastRand() {
	FastRand_Seed = (208413*((FastRand_Seed+4631018)>>1))^(0b01010001110101110101000101101001);
	return FastRand_Seed & 0x7FFFFFFF;
}
///
/// Fast pseudo-random number generator, very inaccurate
/// @returns a random number between 0 and max
///
inline int FastRand(int max) {
	return FastRand() % (max+1);
}
///
/// Fast pseudo-random number generator, very inaccurate
/// @returns a random number between min and max
///
inline int FastRand(int min, int max) {
	return min + (FastRand() % (max-min+1) );
}
///
/// Fast pseudo-random number generator, very inaccurate
/// @returns a random number between 0.f and 1.f
///
inline float FastRandF() {
	return (float)FastRand() / 0x7FFFFFFF;
}


namespace fs {
bool isDir(const std::string &path);
std::string pathCat(const std::string& first, const std::string &second);
std::vector<std::string> getContents(const std::string &path);
std::vector<std::string> getDirs(const std::string &path);
std::vector<std::string> getFiles(const std::string &path);
std::string readFile(const std::string &path);
std::string getParent(const std::string &path);
}

}

#endif