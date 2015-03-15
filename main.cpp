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
#include "Config.hpp"

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

static void showHelp(char **argv) {
	std::cout <<
	"Usage: " << argv[0] << " [options]\n"
	" -h           Shows this help message\n"
	" --help\n\n"
	"Server: -s [-p port]\n"
	" -p port      Specifies port to run server on\n\n"
	"Client: [--nosound] [-n name] [host[:port]]\n"
	" --nosound    Disables sound\n"
	" -n name      Sets player nickname\n"
	" host[:port]  Server (and port) to connect directly to\n"
	<< std::endl;
}

int main(int argc, char **argv) {
	InitRand();

	string host = GlobalProperties::DefaultServerHost;
	int    port = GlobalProperties::DefaultServerPort;
	for (int i=1; i < argc; i++) {
		if (strcmp(argv[i], "-h") == 0 ||
			strcmp(argv[i], "--help") == 0) {
			showHelp(argv);
			return 0;

		} else if (strcmp(argv[i], "-s") == 0) {
			GlobalProperties::IsClient = false;
			GlobalProperties::IsServer = true;
		} else if (strcmp(argv[i], "-p") == 0 && argc > i) {
			if (!GlobalProperties::IsServer) continue;
			try {
				port = std::stoi(argv[++i]);
			} catch (const std::logic_error &e) {
				getErrorStream() << "Failed to parse port number, keeping default (" <<
					port << ')' << std::endl;
			}

		} else if (strcmp(argv[i], "--nosound") == 0) {
			GlobalProperties::IsSoundEnabled = false;
		} else if ((strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "--name") == 0)
			&& argc > i) {
			if (strlen(argv[i+1]) > GlobalProperties::PlayerNameMaxLen) {
				getErrorStream() << "Specified nickname is too long, using random one." << std::endl;
			} else {
				GlobalProperties::PlayerName = argv[++i];
				getDebugStream() << "Player's name set to " << GlobalProperties::PlayerName << std::endl;
		    }
		} else {
			// For now, assume it's the server address
			host = argv[i];
			string::size_type colonPos = host.find(':');
			if (colonPos != string::npos) {
				string portStr = host.substr(colonPos+1);
				try {
					port = std::stoi(portStr);
				} catch (const std::logic_error &e) {
					getErrorStream() << "Failed to parse port number, keeping default (" <<
						port << ')' << std::endl;
				}
				if (port > 65535) {
					port = GlobalProperties::DefaultServerPort;
					getErrorStream() << "Port number too high, defaulting to" <<
						port << std::endl;
				}
				host = host.substr(0, colonPos);
			}
		}
	}

	// TODO: mix up config correctly with all this ^^^^^
	Config cfg;
	cfg.load(getConfigDirectory() + "/config.cfg");

	Game G;
	G.C = &cfg;

	bool networkSuccess = InitNetwork();

	if (GlobalProperties::IsClient) {
		char name[5];
		if (!GlobalProperties::PlayerName) {
			for (int i=0; i < 4; i++)
				name[i] = 'A' + FastRand(25);
			name[4] = '\0';
			GlobalProperties::PlayerName = name;
		}

		GameWindow GW(&G);
		
		/*GW.setNextState(std::make_shared<UITestState>(&GW));*/
		if (networkSuccess)
			GW.setNextState(std::make_shared<GameState>(&GW, host, port));
		else
			GW.setNextState(std::make_shared<MessageState>(&GW, "Network init failed!"));
		GW.run();
	}
	if (GlobalProperties::IsServer) {
		if (!networkSuccess) {
			getErrorStream() << "Network init failed!" << std::endl;
			return 1;
		}
		Server S(G, port);
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
