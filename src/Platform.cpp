#include "Platform.hpp"

#include <cmath>
#include <stdexcept>

#include "util/Log.hpp"

using diggler::Util::Log;
using namespace diggler::Util::Logging::LogLevels;

static const char *TAG = "Platform";

const char *diggler::UserdataDirsName = "Diggler";

static struct PathCache {
  std::string
    executableBin,
    executableDir,
    configDir,
    cacheDir;
} pathCache;

#if defined(BUILDINFO_PLATFORM_WINDOWS) // Windows

#include <cmath>
#include <cstring>
#include <shlobj.h>
#include <windows.h>
#include "platform/Fixes.hpp"

std::string diggler::proc::getExecutablePath() {
  if (pathCache.executableBin.length() == 0) {
    HMODULE hModule = GetModuleHandleW(NULL);
    WCHAR path[MAX_PATH];
    GetModuleFileNameW(hModule, path, MAX_PATH);
    char utf8Path[MAX_PATH];
    WideCharToMultiByte(CP_UTF8, 0, path, -1, utf8Path, MAX_PATH, nullptr, nullptr);
    pathCache.executableBin = utf8Path;
  }
  return pathCache.executableBin;
}

std::string diggler::proc::getExecutableDirectory() {
  if (pathCache.executableDir.length() == 0) {
    std::string filename(getExecutablePath());
    const size_t last_sep_idx = filename.rfind('\\');
    if (last_sep_idx != std::string::npos) {
      pathCache.executableDir = filename.substr(0, last_sep_idx);
    } else {
      Log(Warning, TAG) << "Ill-formed executable path: " << filename;
      pathCache.executableDir = filename;
    }
  }
  return pathCache.executableDir;
}

std::string diggler::getCacheDirectory() {
  if (pathCache.cacheDir.length() == 0) {
    WCHAR ucs2Path[MAX_PATH];
    SHGetFolderPath(nullptr, CSIDL_APPDATA, nullptr, 0, ucs2Path);
    char utf8Path[MAX_PATH];
    WideCharToMultiByte(CP_UTF8, 0, ucs2Path, -1, utf8Path, MAX_PATH, nullptr, nullptr);
    pathCache.cacheDir = std::string(utf8Path) + "/" + UserdataDirsName + "/cache/";
  }
  return pathCache.cacheDir;
}

std::string diggler::getConfigDirectory() {
  if (pathCache.cacheDir.length() == 0) {
    WCHAR ucs2Path[MAX_PATH];
    SHGetFolderPath(nullptr, CSIDL_APPDATA, nullptr, 0, ucs2Path);
    char utf8Path[MAX_PATH];
    WideCharToMultiByte(CP_UTF8, 0, ucs2Path, -1, utf8Path, MAX_PATH, nullptr, nullptr);
    pathCache.configDir = std::string(utf8Path) + "/" + UserdataDirsName + "/config/";
  }
  return pathCache.configDir;
}

#elif defined(BUILDINFO_PLATFORM_UNIXLIKE) // Linux and UNIX alike

#include <sstream>
#include <unistd.h>
#include <dirent.h>
#include <cstdio>

#if defined(BUILDINFO_PLATFORM_MACOS)
#include <cstdint>
#include <sys/syslimits.h>
#include <mach-o/dyld.h>
#endif

std::string do_readlink(const char *path);
std::string do_readlink(const std::string &path);

#if defined(BUILDINFO_PLATFORM_LINUX)
std::string diggler::proc::getExecutablePath() {
  if (pathCache.executableBin.length() == 0) {
    pathCache.executableBin = do_readlink("/proc/self/exe");
  }
  return pathCache.executableBin;
}
#elif defined(BUILDINFO_PLATFORM_MACOS)
std::string diggler::proc::getExecutablePath() {
  if (pathCache.executableBin.length() == 0) {
    char buff[PATH_MAX+1];
    uint32_t size = PATH_MAX+1;
    if (_NSGetExecutablePath(buff, &size) != 0) {
      throw std::runtime_error("Executable path buffer too small");
    }
    pathCache.executableBin = buff;
  }
  return pathCache.executableBin;
}
#endif

std::string diggler::proc::getExecutableDirectory() {
  if (pathCache.executableDir.length() == 0) {
    std::string filename(getExecutablePath());
    const size_t last_slash_idx = filename.rfind('/');
    if (last_slash_idx != std::string::npos) {
      pathCache.executableDir = filename.substr(0, last_slash_idx);
    } else {
      Log(Warning, TAG) << "Ill-formed executable path: " << filename;
      pathCache.executableDir = filename;
    }
  }
  return pathCache.executableDir;
}

std::string diggler::getCacheDirectory() {
  if (pathCache.cacheDir.length() == 0) {
    const char *xdgCache = std::getenv("XDG_CACHE_HOME");
    if (xdgCache) {
      pathCache.cacheDir = std::string(xdgCache) + "/" + UserdataDirsName;
    } else {
      pathCache.cacheDir = std::string(std::getenv("HOME")) + "/.cache/" + UserdataDirsName;
    }
  }
  return pathCache.cacheDir;
}

std::string diggler::getConfigDirectory() {
  if (pathCache.configDir.length() == 0) {
    const char *xdgCache = std::getenv("XDG_CONFIG_HOME");
    if (xdgCache) {
      pathCache.configDir = std::string(xdgCache) + "/" + UserdataDirsName;
    } else {
      pathCache.configDir = std::string(std::getenv("HOME")) + "/.config/" + UserdataDirsName;
    }
  }
  return pathCache.configDir;
}

#else // Any other

// Stub?

#endif

namespace diggler {

std::string getAssetsDirectory() {
  return proc::getExecutableDirectory() + "/assets";
}

std::string getAssetsDirectory(const std::string &type) {
  return proc::getExecutableDirectory() + "/assets/" + type + '/';
}

std::string getAssetPath(const std::string &name) {
  return proc::getExecutableDirectory() + "/assets/" + name;
}

std::string getAssetPath(const std::string &type, const std::string &name) {
  return proc::getExecutableDirectory() + "/assets/" + type + '/' + name;
}

float rmod(float x, float y) {
  float ret = fmod(x, y);
  if (ret < 0)
    return y+ret;
  return ret;
}

double rmod(double x, double y) {
  double ret = fmod(x, y);
  if (ret < 0)
    return y+ret;
  return ret;
}

}
