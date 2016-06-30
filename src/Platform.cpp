#include "Platform.hpp"

#include <fstream>
#include <thread>
#include <mutex>
#include <functional>
#include <climits>
#include <cinttypes>
#include <random>
#include <chrono>
#include <cstring>

#include <GLFW/glfw3.h>

const char *Diggler::UserdataDirsName = "Diggler";
uint Diggler::FastRand_Seed = 0;

static struct PathCache {
  std::string
    executableBin,
    executableDir,
    configDir,
    cacheDir;
} pathCache;

#ifdef BUILDINFO_PLATFORM_WINDOWS // Windows

// Put windows crap here

#elif defined(BUILDINFO_PLATFORM_UNIXLIKE) // Linux and UNIX alike

#include <sstream>
#include <unistd.h>
#include <dirent.h>
#include <cstdio>

std::string do_readlink(const char *path) throw(int) {
  char buff[PATH_MAX+1];
  ssize_t len = readlink(path, buff, sizeof(buff) - 1);
  if (len >= 0) {
    buff[len] = '\0';
    return std::string(buff);
  } else {
    throw len;
  }
}
std::string do_readlink(const std::string &path) throw(int) {
  if (path.length() > PATH_MAX)
    throw -1;
  return do_readlink(path.c_str());
}

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
      getErrorStream() << "Ill-formed executable path: " << filename << std::endl;
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


std::string Diggler::fs::pathCat(const std::string &first, const std::string &second) {
  if(first.at(first.length()-1) == '/')
    return first + second;
  return first + '/' + second;
}

std::string Diggler::fs::getParent(const std::string &path) {
  return path.substr(path.find_last_of('/'));
}

std::vector<std::string> Diggler::fs::getContents(const std::string &path) {
  DIR *dir = opendir(path.c_str());
  if(dir == 0)
    return std::vector<std::string>();
  std::vector<std::string> entitys = std::vector<std::string>();
  dirent *entity;
  while((entity = readdir(dir)) != nullptr) {
    if(entity->d_name[0] == '.' && (entity->d_name[1] == 0 || (entity->d_name[1] == '.' && entity->d_name[2] == 0)))
      continue;
    entitys.push_back(entity->d_name);
  }
  closedir(dir);
  return entitys;
}

std::vector<std::string> Diggler::fs::getDirs(const std::string &path) {
  DIR *dir = opendir(path.c_str());
  if(dir == 0)
    return std::vector<std::string>();
  std::vector<std::string> entitys = std::vector<std::string>();
  dirent *entity;
  while((entity = readdir(dir)) != nullptr) {
    if(entity->d_name[0] == '.' && (entity->d_name[1] == 0 || (entity->d_name[1] == '.' && entity->d_name[2] == 0)))
      continue;
    if(isDir(pathCat(path, entity->d_name)))
      entitys.push_back(entity->d_name);
  }
  closedir(dir);
  return entitys;
}

std::vector<std::string> Diggler::fs::getFiles(const std::string &path) {
  DIR *dir = opendir(path.c_str());
  if(dir == 0)
    return std::vector<std::string>();
  std::vector<std::string> entitys = std::vector<std::string>();
  dirent *entity;
  while((entity = readdir(dir)) != nullptr) {
    if(entity->d_name[0] == '.' && (entity->d_name[1] == 0 || (entity->d_name[1] == '.' && entity->d_name[2] == 0)))
      continue;
    if(!isDir(pathCat(path, entity->d_name)))
      entitys.push_back(entity->d_name);
  }
  closedir(dir);
  return entitys;
}

inline bool Diggler::fs::isDir(const std::string &path) {
  DIR *dir = opendir(path.c_str());
  if(dir)
  {
    closedir(dir);
    return true;
  }
  return false;
}

#elif defined(BUILDINFO_PLATFORM_MAC) // Mac

// Put Mac code here

#else // Any other

// Stub?

#endif

namespace Diggler {

std::string fs::readFile(const std::string &path) {
  std::ifstream in(path, std::ios::in | std::ios::binary);
  if (in) {
    std::string contents;
    in.seekg(0, std::ios::end);
    contents.resize(in.tellg());
    in.seekg(0, std::ios::beg);
    in.read(&contents[0], contents.size());
    in.close();
    return contents;
  }
  return "";
}

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

std::ostream& getErrorStreamRaw() {
  return std::cerr;
}

std::ostream& getDebugStreamRaw() {
  return std::cout;
}

std::ostream& getOutputStreamRaw() {
  return std::cout;
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
