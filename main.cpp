#include <iostream>
#include <memory>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <thread>
#include <chrono>
#include <sys/signal.h>
#include "Game.hpp"
#include "GameWindow.hpp"
#include "GameState.hpp"
#include "MessageState.hpp"
#include "GlobalProperties.hpp"
#include "Server.hpp"
#include "network/Network.hpp"


#include "UITestState.hpp"

namespace Diggler {

using std::string;

static bool InitNetwork() {
	std::cout << Net::GetNetworkLibVersion() << std::endl;
	return Net::Init();
}

static void InitRand() {
	FastRandSeed(std::time(nullptr););
	int n = FastRand(128);
	for (int i=0; i < n; ++i)
		FastRand();
}

int main(int argc, char **argv) {
	InitRand();
	bool networkSuccess = InitNetwork();
	
	string servHost = GlobalProperties::DefaultServerHost;
	int    servPort = GlobalProperties::DefaultServerPort;
	for (int i=1; i < argc; i++) {
		if (strcmp(argv[i], "--server") == 0) {
			GlobalProperties::IsClient = false;
			GlobalProperties::IsServer = true;
		} else if (strcmp(argv[i], "--nosound") == 0) {
			GlobalProperties::IsSoundEnabled = false;
		} else {
			// For now, assume it's the server address
			servHost = argv[i];
			string::size_type colonPos = servHost.find(':');
			if (colonPos != string::npos) {
				string portStr = servHost.substr(colonPos+1);
				try {
					servPort = std::stoi(portStr);
				} catch (const std::logic_error &e) {
					getErrorStream() << "Failed to parse port number, keeping default (" <<
						servPort << ')' << std::endl;
				}
				servHost = servHost.substr(0, colonPos);
			}
		}
	}
	
	if (GlobalProperties::IsClient) {
		GameWindow GW;
		/*GW.setNextState(std::make_shared<UITestState>(&GW));*/
		if (networkSuccess)
			GW.setNextState(std::make_shared<GameState>(&GW, servHost, servPort));
		else
			GW.setNextState(std::make_shared<MessageState>(&GW, "Network init failed!"));
		GW.run();
	}
	if (GlobalProperties::IsServer) {
		if (!networkSuccess) {
			getErrorStream() << "Network init failed!" << std::endl;
			return 1;
		}
		Game G;
		Server S(&G);
		G.S = &S;
		S.run();
	}
	
	return 0;
}

}

int main(int argc, char **argv) {
	//try {
		return Diggler::main(argc, argv);
	/*} catch (std::exception &e) {
		std::cerr << "==== CRASHED (std::exception) ===" << std::endl << e.what() << std::endl;
	} catch (std::string &e) {
		std::cerr << "==== CRASHED (std::string) ===" << std::endl << e << std::endl;
	} catch (const char *e) {
		std::cerr << "==== CRASHED (const char*) ===" << std::endl << e << std::endl;
	} catch (...) {
		std::cerr << "==== CRASHED ===" << std::endl;
	}*/
	//return 0;
}
