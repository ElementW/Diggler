#ifndef GLOBAL_PROPERTIES_HPP
#define GLOBAL_PROPERTIES_HPP

namespace diggler {

namespace GlobalProperties {
  extern bool IsClient;
  extern bool IsServer;

  extern const char *DefaultServerHost;
  extern const int   DefaultServerPort;

  extern const unsigned int PlayerNameMaxLen;
  extern char *PlayerName;

  extern bool IsSoundEnabled;

  extern int UIScale;
}

}

#endif
