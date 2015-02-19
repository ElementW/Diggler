#include "GlobalProperties.hpp"

namespace Diggler {

bool GlobalProperties::IsClient = true;
bool GlobalProperties::IsServer = false;

const char* GlobalProperties::DefaultServerHost = "localhost";
const int   GlobalProperties::DefaultServerPort = 61425;

bool GlobalProperties::UseProceduralTextures = false;

bool GlobalProperties::IsSoundEnabled = true;

int GlobalProperties::UIScale = 2;


}