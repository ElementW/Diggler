#include "Platform.hpp"

#include <cmath>

#include "util/Log.hpp"

using Diggler::Util::Log;
using namespace Diggler::Util::Logging::LogLevels;

static const char *TAG = "Platform";

const char *Diggler::UserdataDirsName = "Diggler";

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

std::string Diggler::proc::getExecutablePath() {
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

std::string Diggler::proc::getExecutableDirectory() {
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

std::string Diggler::getCacheDirectory() {
  if (pathCache.cacheDir.length() == 0) {
    WCHAR ucs2Path[MAX_PATH];
    SHGetFolderPath(nullptr, CSIDL_APPDATA, nullptr, 0, ucs2Path);
    char utf8Path[MAX_PATH];
    WideCharToMultiByte(CP_UTF8, 0, ucs2Path, -1, utf8Path, MAX_PATH, nullptr, nullptr);
    pathCache.cacheDir = std::string(utf8Path) + "/" + UserdataDirsName + "/cache/";
  }
  return pathCache.cacheDir;
}

std::string Diggler::getConfigDirectory() {
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

std::string do_readlink(const char *path);
std::string do_readlink(const std::string &path);

#if defined(BUILDINFO_PLATFORM_LINUX)
std::string Diggler::proc::getExecutablePath() {
  if (pathCache.executableBin.length() == 0) {
    pid_t pid = getpid();
    // Assuming 32-bit pid -> max of 10 digits, we need only "/proc/xxxxxxxxxx/exe" space
    char path[21];
    std::snprintf(path, 21, "/proc/%d/exe", pid);
    pathCache.executableBin = do_readlink(path);
  }
  return pathCache.executableBin;
}
#else
// TODO: getExecutablePath for those without procfs
#endif

std::string Diggler::proc::getExecutableDirectory() {
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

std::string Diggler::getCacheDirectory() {
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

std::string Diggler::getConfigDirectory() {
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

#elif defined(BUILDINFO_PLATFORM_MAC) // Mac

// Put Mac code here

#else // Any other

// Stub?

#endif

namespace Diggler {

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
