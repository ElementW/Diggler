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

std::string ProgramManager::getShadersName(FlagsT flags) {
  if (flags & PM_3D)
    return "3d";
  else
    return "2d";
}

void ProgramManager::getPreludeLines(FlagsT flags, std::vector<std::string> &lines) {
  auto addDefine = [&lines](const char* d) { lines.push_back(std::string("#define ") + d); };
  if (flags & PM_TEXTURED)
    addDefine("TEXTURED");
  if (flags & PM_TEXSHIFT)
    addDefine("TEXSHIFT");
  if (flags & PM_COLORED)
    addDefine("COLORED");
  if (flags & PM_FOG)
    addDefine("FOG");
  if (flags & PM_DISCARD)
    addDefine("DISCARD");
  if (flags & PM_TIME)
    addDefine("TIME");
  if (flags & PM_WAVE)
    addDefine("WAVE");
  if (flags & PM_POINTSIZE)
    addDefine("POINTSIZE");
  if (flags & PM_EARLY_DEPTH_TEST)
    lines.push_back("layout(early_fragment_tests) in;");
}

ProgramManager::ProgramManager(Game &G) : G(G) {

}

const Program* ProgramManager::getProgram(FlagsT flags) {
  auto it = m_programs.find(flags);
  if (it != m_programs.end())
    return it->second;
  std::string shaderName = getShadersName(flags);
  Program *prog = new Program(getAssetPath(shaderName + ".v.glsl"), getAssetPath(shaderName + ".f.glsl"));
  std::vector<std::string> preludeLines;
  getPreludeLines(flags, preludeLines);
  prog->setPreludeLines(preludeLines);
  if (!prog->link()) {
    Log(Error, TAG) << "Link failed on " << shaderName;
    if (preludeLines.size() > 0) {
      std::ostringstream oss;
      for (const std::string &line : preludeLines) {
        oss << line << std::endl;
      }
      Log(Error, TAG) << "Prelude: " << oss.str();
    }
    /*FIXME: use in debug? GLint sz;
    glGetShaderiv(prog->getVShId(), GL_SHADER_SOURCE_LENGTH, &sz);=
    char *src = new char[sz+1];
    src[sz] = 0;
    glGetShaderSource(prog->getVShId(), sz, nullptr, src);
    getErrorStream() << "Source: " << src << std::endl;
    delete[] src; */
  }
  m_programs.insert(std::pair<int, Program*>(flags, prog));
#if PROGRAM_MANAGER_DEBUG
  getDebugStream() << "Added " << shaderName << ':' << prog->getId() << std::endl;
#endif
  return prog;
}

const Program* ProgramManager::getSpecialProgram(const std::string &name) {
  Program* prog = new Program(getAssetPath(name + ".v.glsl"), getAssetPath(name + ".f.glsl"));
  prog->link();
  m_specialPrograms.push_back(prog);
#if PROGRAM_MANAGER_DEBUG
  getDebugStream() << "AddSpecial " << name << ':' << prog->getId() << std::endl;
#endif
  return prog;
}

ProgramManager::~ProgramManager() {
  for (const std::pair<FlagsT, Program*> pair : m_programs) {
    delete pair.second;
  }
  for (const Program *prog : m_specialPrograms) {
    delete prog;
  }
}

}
}
}
