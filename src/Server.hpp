#ifndef SERVER_HPP
#define SERVER_HPP
#include <memory>
#include <lua.hpp>
#include "network/Network.hpp"
#include "Player.hpp"

using std::unique_ptr;

namespace Diggler {

class Game;

class Server {
private:
  Game &G;
  lua_State *L;

  // TODO: REMOVEME!!!
  std::list<ChunkRef> holdChunksInMem;

  void handleCommand(Player*, const std::string &command, const std::vector<std::string> &args);

  void handlePlayerJoin(Net::InMessage&, Net::Peer&);
  void handlePlayerQuit(Net::Peer&, Net::QuitReason reason = Net::QuitReason::Quit);
  void handleDisconnect(Net::Peer&);

  void handleChat(Net::InMessage&, Player*);

  void handleEvent(Net::InMessage&, Net::Peer&);
  void handlePlayerUpdate(Net::InMessage&, Player&);
    void handlePlayerDeath(Net::InMessage&, Player&);
  void handlePlayerChunkRequest(Net::InMessage&, Player&);
  void handlePlayerMapUpdate(Net::InMessage&, Player&);

  void schedSendChunk(ChunkRef, Player&);
  void sendChunks(const std::list<ChunkRef>&, Player&);

  void chunkUpdater(WorldRef WR, bool &continueUpdate);

public:
  Net::Host H;

  Server(Game &G, uint16 port);
  ~Server();
  void startInternals();
  void start();
  void run();
  void stop();
  void stopInternals();

  bool isPlayerOnline(const std::string &playername) const;
  bool isIPOnline(const std::string &ip) const;
  Player* getPlayerById(uint32 id);
  Player* getPlayerByPeer(const Net::Peer &peer);
  Player* getPlayerByName(const std::string &name);
  void kick(Player &p, Net::QuitReason r = Net::QuitReason::Kicked, const std::string& message = "");
};

}

#endif
