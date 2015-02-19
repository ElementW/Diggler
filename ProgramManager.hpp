#ifndef PROGRAM_MANAGER_HPP
#define PROGRAM_MANAGER_HPP
#include <unordered_map>
#include <vector>
#include "Program.hpp"

#define PM_2D 0
#define PM_3D 1
#define PM_TEXTURED 2
#define PM_COLORED 4
#define PM_FOG 8
#define PM_TEXSHIFT 16

namespace Diggler {

class ProgramManager {
private:
	std::unordered_map<int, Program*> m_programs;
	std::vector<Program*> m_specialPrograms;
	std::string getShadersName(int flags) const;

public:
	ProgramManager();
	~ProgramManager();
	const Program* getProgram(int flags);
	const Program* getSpecialProgram(const std::string &name);
};

}

#endif