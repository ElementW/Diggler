#ifndef DIGGLER_PLATFORM_FS_HPP
#define DIGGLER_PLATFORM_FS_HPP

#include <string>
#include <vector>

namespace Diggler {
namespace fs {

/**
 * @returns `true` if specified `path` is a directory, `false` otherwise
 */
bool isDir(const std::string &path);

/** Concatenates two path parts
 * Concatenates two paths avoiding path element separator duplication.
 * @returns Concatenated path
 */
std::string pathCat(const std::string& first, const std::string &second);

/** Enumerates a given directory `path`'s content
 * @returns Vector of each element's name
 */
std::vector<std::string> getContents(const std::string &path);

/** Enumerates a given directory `path`'s subdirectories
 * @returns Vector of each subdirectory's name
 */
std::vector<std::string> getDirs(const std::string &path);

/** Enumerates a given directory `path`'s files
 * @returns Vector of each file's name
 */
std::vector<std::string> getFiles(const std::string &path);

/** Reads a whole file denoted by `path` and returns its content
 * Only works if element filesystem entry `path` is a normal file.
 * Directories, FIFOs (pipes), block/character devices files aren't supported and
 * will return an empty string.
 * Softlinks are followed.
 * @returns `path`'s content, or an empty string if `path` isn't a normal file.
 */
std::string readFile(const std::string &path);

/** Get `path`'s hierarchical parent in the filesystem.
 * @note No symlinks are resolved in the operation, and getting the parent of the
 *       topmost directory (root) will return itself as its parent.
 *       Also, parent directories may be unreadable due to filesystem permissions.
 * @return `path`'s filesystem parent path
 */
std::string getParent(const std::string &path);

}
}

#endif /* DIGGLER_PLATFORM_FS_HPP */
