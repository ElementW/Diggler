#ifndef PROGRAM_MANAGER_HPP
#define PROGRAM_MANAGER_HPP
#include <unordered_map>
#include <vector>
#include "Program.hpp"

#define PM_2D			0x0
#define PM_3D			0x1
#define PM_TEXTURED		0x2
#define PM_COLORED		0x4
#define PM_FOG			0x8
#define PM_TEXSHIFT		0x10
#define PM_DISCARD		0x20
#define PM_TIME			0x40
#define PM_WAVE			(0x80 | PM_TIME)
#define PM_POINTSIZE	0x100

namespace Diggler {

class ProgramManager {
public:
	typedef uint32_t FlagsT;

private:
	class Game &G;
	std::unordered_map<FlagsT, Program*> m_programs;
	std::vector<Program*> m_specialPrograms;
	static std::string getShadersName(FlagsT flags);
	static void getDefines(FlagsT flags, std::vector<std::string> &defs);

public:
	ProgramManager(Game&);
	~ProgramManager();
	const Program* getProgram(FlagsT flags);
	const Program* getSpecialProgram(const std::string &name);
};

}

#endif