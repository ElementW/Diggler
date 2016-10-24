#ifndef PLATFORM_HPP
#define PLATFORM_HPP

#include <vector>
#include <string>
#include <iostream>

#include "platform/BuildInfo.hpp"
#include "platform/FastRand.hpp"
#include "platform/FourCC.hpp"
#include "platform/fs.hpp"
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

}

#endif
