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

double (*Diggler::getTime)() = glfwGetTime;


#ifdef BUILDINFO_PLATFORM_WINDOWS // Windows

// Put windows crap here

#elif defined(BUILDINFO_PLATFORM_LINUX) || defined(BUILDINFO_PLATFORM_ANDROID) // Linux

#include <sstream>
#include <unistd.h>
#include <dirent.h>
#include <cstdio>

std::string do_readlink(const char *path) throw(int) {
	char buff[PATH_MAX];
	ssize_t len = readlink(path, buff, sizeof(buff) - 1);
	if(len >= 0) {
		buff[len] = '\0';
		return std::string(buff);
	} else {
		throw len;
	}
}
std::string do_readlink(const std::string &path) throw(int) {
	if(path.length() > PATH_MAX)
		throw -1;
	return do_readlink(path.c_str());
}

bool buildCachedExecutablePath = true;
std::string cachedExecutablePath;

std::string Diggler::getExecutablePath() {
	if(buildCachedExecutablePath) {
		pid_t pid = getpid();
		// Assuming 32-bit pid -> max of 10 digits, we need only "/proc/xxxxxxxxxx/exe" space
		char path[22];
		std::sprintf(path, "/proc/%d/exe", pid);
		cachedExecutablePath = do_readlink(path);
		buildCachedExecutablePath = false;
	}
	return cachedExecutablePath;
}

bool buildCachedExecutableDirectory = true;
std::string cachedExecutableDirectory;

std::string Diggler::getExecutableDirectory() {
	if(buildCachedExecutableDirectory) {
		std::string filename(getExecutablePath());
		const size_t last_slash_idx = filename.rfind('/');
		if(last_slash_idx != std::string::npos) {
			cachedExecutableDirectory = filename.substr(0, last_slash_idx + 1);
		} else {
			getErrorStream() << "Ill-formed executable path: " << filename << std::endl;
			cachedExecutableDirectory = filename;
		}
		buildCachedExecutableDirectory = false;
	}
	return cachedExecutableDirectory;
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

// Put Mac crap here

#else // Any other

//Put other craps here

#endif

std::string Diggler::fs::readFile(const std::string &path) {
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

std::string Diggler::getAssetsDirectory() {
	return Diggler::getExecutableDirectory() + "assets/";
}

std::string Diggler::getAssetsDirectory(const std::string &type) {
	return Diggler::getExecutableDirectory() + "assets/" + type + '/';
}

std::string Diggler::getAssetPath(const std::string &name) {
	return Diggler::getExecutableDirectory() + "assets/" + name;
}

std::string Diggler::getAssetPath(const std::string &type, const std::string &name) {
	return Diggler::getExecutableDirectory() + "assets/" + type + '/' + name;
}

std::ostream& Diggler::getErrorStreamImpl() {
	return std::cerr;
}

std::ostream& Diggler::getDebugStreamImpl() {
	return std::cout;
}

std::ostream &Diggler::getOutputStreamImpl() {
	return std::cout;
}

uint Diggler::FastRand_Seed = 0;
