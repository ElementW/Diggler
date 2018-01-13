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

namespace diggler {

namespace proc {
  /// @returns The executable's absolute path
  std::string getExecutablePath();

  /// @returns The executable's absolute path directory, including the end slash (/)
  std::string getExecutableDirectory();
}

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
