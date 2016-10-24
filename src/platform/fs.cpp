#include "fs.hpp"

#include <fstream>

#include "BuildInfo.hpp"

#if defined(BUILDINFO_PLATFORM_WINDOWS) // Windows

// Put windows crap here

#elif defined(BUILDINFO_PLATFORM_UNIXLIKE) // Linux and UNIX alike

#include <sstream>
#include <unistd.h>
#include <dirent.h>
#include <cstdio>

std::string do_readlink(const char *path) {
  char buff[PATH_MAX+1];
  ssize_t len = readlink(path, buff, sizeof(buff) - 1);
  if (len >= 0) {
    buff[len] = '\0';
    return std::string(buff);
  } else {
    throw len;
  }
}
std::string do_readlink(const std::string &path) {
  if (path.length() > PATH_MAX)
    throw -1;
  return do_readlink(path.c_str());
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

#else // Any other

// Stub?

#endif

namespace Diggler {
namespace fs {

std::string readFile(const std::string &path) {
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

}
}
