#include <cstring>
#include <iostream>
#include <memory>
#include <thread>
#include <chrono>

#include "ConnectingState.hpp"
#include "Game.hpp"
#include "GameWindow.hpp"
#include "MessageState.hpp"
#include "GlobalProperties.hpp"
#include "Server.hpp"
#include "network/Network.hpp"
#include "Config.hpp"
#include "util/Log.hpp"
#include "util/MemoryTracker.hpp"

#include "UITestState.hpp"

namespace Diggler {

using Util::Log;
using namespace Util::Logging::LogLevels;

static const char *TAG = "main()";

using std::string;

static bool InitNetwork() {
  std::cout << Net::GetNetworkLibVersion() << std::endl;
  return Net::Init();
}

static void InitRand() {
  using Time = std::chrono::high_resolution_clock;
  using ms = std::chrono::milliseconds;
  FastRandSeed(std::chrono::duration_cast<ms>(Time::now().time_since_epoch()).count()%0x7FFFFFFF);
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

template<class T>
struct default_destruct final {
  void operator()(T *ptr) {
    ptr->~T();
  }
};

int main(int argc, char **argv) {
  Util::MemoryTracker::init();
  InitRand();
  Util::InitLogging();

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
        Log(Error, TAG) << "Failed to parse port number, keeping default (" << port << ')';
      }

    } else if (strcmp(argv[i], "--nosound") == 0) {
      GlobalProperties::IsSoundEnabled = false;
    } else if ((strcmp(argv[i], "-n") == 0 || strcmp(argv[i], "--name") == 0)
      && argc > i) {
      if (strlen(argv[i+1]) > GlobalProperties::PlayerNameMaxLen) {
        Log(Warning, TAG) << "Specified nickname is too long, using random one.";
      } else {
        GlobalProperties::PlayerName = argv[++i];
        Log(Info, TAG) << "Player's name set to " << GlobalProperties::PlayerName;
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
          Log(Error, TAG) << "Failed to parse port number, keeping default (" << port << ')';
        }
        if (port > 65535) {
          port = GlobalProperties::DefaultServerPort;
          Log(Warning, TAG) << "Port number too high, defaulting to" << port;
        }
        host = host.substr(0, colonPos);
      }
    }
  }

  // TODO: mix up config correctly with all this ^^^^^
  Config cfg;
  cfg.load(getConfigDirectory() + "/config.cfg");

  alignas(alignof(Game)) uint8 G_mem[sizeof(Game)];
  std::unique_ptr<Game, default_destruct<Game>> G(new (G_mem) Game);
  G->C = &cfg;

  bool networkSuccess = InitNetwork();

  if (GlobalProperties::IsClient) {
    char name[5];
    if (!GlobalProperties::PlayerName) {
      for (int i=0; i < 4; i++)
        name[i] = 'A' + FastRand(25);
      name[4] = '\0';
      GlobalProperties::PlayerName = name;
    }

    {
      GameWindow GW(G.get());

      /*/GW.setNextState(std::make_shared<UITestState>(&GW));/*/
      if (networkSuccess)
        GW.setNextState(std::make_unique<ConnectingState>(&GW, host, port));
      else
        GW.showMessage("Network init failed!");
      /**/

      GW.run();
      G->finalizeClient();
    }
    G.reset();
  }
  if (GlobalProperties::IsServer) {
    if (!networkSuccess) {
      Log(Failure, TAG) << "Network init failed!";
      return 1;
    }
    Server S(*G, port);
    G->S = &S;
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
