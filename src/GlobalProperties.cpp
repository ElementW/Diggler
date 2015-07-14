#include "GlobalProperties.hpp"

namespace Diggler {

bool GlobalProperties::IsClient = true;
bool GlobalProperties::IsServer = false;

const char *GlobalProperties::DefaultServerHost = "localhost";
const int   GlobalProperties::DefaultServerPort = 17425;

const unsigned int GlobalProperties::PlayerNameMaxLen = 30;
char *GlobalProperties::PlayerName = nullptr;

bool GlobalProperties::IsSoundEnabled = true;

int GlobalProperties::UIScale = 2;


}
