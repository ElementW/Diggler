#include "ProgramManager.hpp"

#include <sstream>

#include "../../Game.hpp"
#include "../../Platform.hpp"
#include "../../util/Log.hpp"

#define PROGRAM_MANAGER_DEBUG 0

namespace Diggler {
namespace Render {
namespace gl {

using Util::Log;
using namespace Util::Logging::LogLevels;

static const char *TAG = "ProgramManager";

static void getPreludeLines(const std::set<std::string> &enables, std::vector<std::string> &lines) {
  auto has = [&enables](const char *s) -> bool { return enables.find(s) != enables.cend(); };
  auto addDefine = [&lines](const char *d) { lines.push_back(std::string("#define ") + d); };
  if (has("texture0"))
    addDefine("TEXTURED");
  if (has("texshift0"))
    addDefine("TEXSHIFT");
  if (has("color0"))
    addDefine("COLORED");
  if (has("fog0"))
    addDefine("FOG");
  if (has("discard"))
    addDefine("DISCARD");
  if (has("time"))
    addDefine("TIME");
  if (has("wave"))
    addDefine("WAVE");
  if (has("earlyDepthTest"))
    lines.push_back("layout(early_fragment_tests) in;");
}

ProgramManager::ProgramManager(Game &G) :
  G(G) {
}

const Program* ProgramManager::getProgram(const std::string &name,
    const std::set<std::string> &enables) {
  ProgramMetadata meta;
  meta.name = name;
  meta.enabledBindings = enables;
  auto it = m_programs.find(meta);
  if (it != m_programs.end()) {
    return it->second.get();
  }
  Program *prog = new Program(getAssetPath(name + ".v.glsl"), getAssetPath(name + ".f.glsl"));
  std::vector<std::string> preludeLines;
  getPreludeLines(enables, preludeLines);
  prog->setPreludeLines(preludeLines);
  if (!prog->link()) {
    Log(Error, TAG) << "Link failed on " << name;
    if (preludeLines.size() > 0) {
      std::ostringstream oss;
      for (const std::string &line : preludeLines) {
        oss << line << std::endl;
      }
      Log(Error, TAG) << "Prelude: " << oss.str();
    }
  }
  m_programs.insert(std::pair<ProgramMetadata, std::unique_ptr<Program>>(meta, prog));
#if PROGRAM_MANAGER_DEBUG
  Log(Debug, TAG) << "Added " << shaderName << ':' << prog->getId();
#endif
  return prog;
}

ProgramManager::~ProgramManager() {
}

}
}
}
