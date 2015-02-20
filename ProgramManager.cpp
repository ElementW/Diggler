#include "ProgramManager.hpp"
#include <sstream>
#include "Platform.hpp"
#include "GlUtils.hpp"

#define PROGRAM_MANAGER_DEBUG 0

namespace Diggler {

std::string ProgramManager::getShadersName(int flags) {
	if (flags & PM_3D)
		return "3d";
	else
		return "2d";
}

void ProgramManager::getDefines(int flags, std::vector<std::string> &defs) {
	if (flags & PM_TEXTURED)
		defs.push_back("TEXTURED");
	if (flags & PM_TEXSHIFT)
		defs.push_back("TEXSHIFT");
	if (flags & PM_COLORED)
		defs.push_back("COLORED");
	if (flags & PM_FOG)
		defs.push_back("FOG");
}

ProgramManager::ProgramManager() {

}

const Program* ProgramManager::getProgram(int flags) {
	auto it = m_programs.find(flags);
	if (it != m_programs.end())
		return it->second;
	std::string shaderName = getShadersName(flags);
	Program* prog = new Program(getAssetPath(shaderName + ".v.glsl"), getAssetPath(shaderName + ".f.glsl"));
	std::vector<std::string> defs;
	getDefines(flags, defs);
	prog->setDefines(defs);
	if (!prog->link()) {
		getErrorStream() << "Link failed on " << shaderName << std::endl;
		if (defs.size() > 0) {
			std::ostringstream oss;
			for (const std::string &s : defs)
				oss << s << "; ";
			getErrorStream() << "Defs: " << oss.str() << std::endl;
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
	for (const std::pair<const int, Program*> pair : m_programs) {
		delete pair.second;
	}
	for (const Program* prog : m_specialPrograms) {
		delete prog;
	}
}

}