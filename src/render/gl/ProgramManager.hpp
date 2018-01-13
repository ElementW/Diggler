#ifndef DIGGLER_RENDER_GL_PROGRAM_MANAGER_HPP
#define DIGGLER_RENDER_GL_PROGRAM_MANAGER_HPP

#include <memory>
#include <set>
#include <unordered_map>
#include <utility>
#include <vector>

#include "Program.hpp"

namespace diggler {

class Game;

namespace render {
namespace gl {

struct ProgramMetadata {
  std::string name;
  std::set<std::string> enabledBindings;

  bool operator==(const ProgramMetadata &o) const {
    return name == o.name && enabledBindings == o.enabledBindings;
  }
};

}
}
}

namespace std {

template <>
class hash<diggler::render::gl::ProgramMetadata> {
public:
size_t operator()(const diggler::render::gl::ProgramMetadata &meta) const {
  std::size_t hash = std::hash<std::string>{}(meta.name);
  for (const std::string &s : meta.enabledBindings) {
    hash ^= (std::hash<std::string>{}(s) << 1);
  }
  return hash;
}
};

}

namespace diggler {
namespace render {
namespace gl {

class ProgramManager {
private:
  Game &G;
  std::unordered_map<ProgramMetadata, std::unique_ptr<Program>> m_programs;

public:
  ProgramManager(Game&);
  ~ProgramManager();

  const Program* getProgram(const std::string &name, const std::set<std::string> &enables);
  const Program* getProgram(const std::string &name) {
    return getProgram(name, {});
  }
  template<typename... Args> const Program* getProgram(const std::string &name,
      const std::string arg0, Args&&... args) {
    std::set<std::string> enables = { arg0, std::forward<Args>(args)... };
    return getProgram(name, enables);
  }
};

}
}
}

#endif /* DIGGLER_RENDER_GL_PROGRAM_MANAGER_HPP */
