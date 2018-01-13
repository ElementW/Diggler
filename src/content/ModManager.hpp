#ifndef DIGGLER_CONTENT_MOD_MANAGER_HPP
#define DIGGLER_CONTENT_MOD_MANAGER_HPP

#include "Mod.hpp"

namespace diggler {

class Game;

namespace content {

class ModManager final {
private:
  Game *G;

public:
  ModManager(Game*);

  /**
   * @brief Resolves a mod-relative or absolute path into a filesystem path
   *        Accepted schemes: builtin, mod:*, self
   * @param mod Mod context
   * @param path Mod-relative or absolute path
   * @return Resolved filesystem path
   */
  std::string filesystemPath(const Mod &mod, const std::string &path);

  /**
   * @brief Resolves an absolute path into a filesystem path
   *        Accepted schemes: builtin, mod:*
   * @param path Absolute path
   * @return Resolved filesystem path
   */
  std::string filesystemPath(const std::string &path);


  /**
   * @brief Determines if the path is absolute or mod-relative (i.e. requires a mod context to be
   *        resolved)
   * @param path Path to check
   * @return true if path is absolute, false otherwise
   */
  static bool isAbsolutePath(const std::string &path);

  /**
   * @brief Resolves a mod-relative path into an absolute path
   *        Accepted schemes: builtin, mod:*, self
   * @param path Mod-relative path
   * @return Resolved filesystem path
   */
  std::string absolutePath(const Mod &mod, const std::string &path);
};

}
}

#endif /* DIGGLER_CONTENT_MOD_MANAGER_HPP */
