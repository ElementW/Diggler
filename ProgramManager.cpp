#include "ProgramManager.hpp"
#include <sstream>
#include "Platform.hpp"

#define PROGRAM_MANAGER_DEBUG 0

namespace Diggler {

std::string ProgramManager::getShadersName(int flags) const {
	std::ostringstream sstm;
	if (flags & PM_3D)
		sstm << "3d";
	else
		sstm << "2d";
	if (flags & PM_TEXTURED)
		sstm << "Textured";
	if (flags & PM_TEXSHIFT)
		sstm << "Texshift";
	if (flags & PM_COLORED)
		sstm << "Colored";
	if (flags & PM_FOG)
		sstm << "Fog";
	return sstm.str();
}

ProgramManager::ProgramManager() {

}

const Program* ProgramManager::getProgram(int flags) {
	auto it = m_programs.find(flags);
	if (it != m_programs.end())
		return it->second;
	std::string shaderName = getShadersName(flags);
	Program* prog = new Program(getAssetPath(shaderName + ".v.glsl"), getAssetPath(shaderName + ".f.glsl"));
	prog->link();
	m_programs.insert(std::pair<int, Program*>(flags, prog));
#if PROGRAM_MANAGER_DEBUG
	getDebugStream() << "Added " << shaderName << ':' << prog->getId() << std::endl;
#endif
	return prog;
}

const Program *ProgramManager::getSpecialProgram(const std::string &name) {
	Program* prog = new Program(getAssetPath(name + ".v.glsl"), getAssetPath(name + ".f.glsl"));
	prog->link();
	m_specialPrograms.push_back(prog);
#if PROGRAM_MANAGER_DEBUG
	getDebugStream() << "AddSpecial " << name << ':' << prog->getId() << std::endl;
#endif
	return prog;
}

ProgramManager::~ProgramManager() {
	for (const std::pair<const int, Program*> pair : m_programs) {
		delete pair.second;
	}
	for (const Program* prog : m_specialPrograms) {
		delete prog;
	}
}

}