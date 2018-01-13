#ifndef SERVER_HPP
#define SERVER_HPP

#include <memory>

#include "network/Network.hpp"
#include "Player.hpp"

using std::unique_ptr;

namespace diggler {

class Game;

class Server {
private:
  Game &G;

  // TODO: REMOVEME!!!
  std::list<ChunkRef> holdChunksInMem;

  void handleCommand(Player*, const std::string &command, const std::vector<std::string> &args);

  void handlePlayerJoin(net::InMessage&, net::Peer&);
  void handlePlayerQuit(net::Peer&, net::QuitReason reason = net::QuitReason::Quit);
  void handleDisconnect(net::Peer&);

  void handleContentMessage(net::InMessage&, net::Peer&);

  void handleChat(net::InMessage&, Player*);

  void handlePlayerUpdate(net::InMessage&, Player&);
    void handlePlayerDeath(net::InMessage&, Player&);
  void handlePlayerChunkRequest(net::InMessage&, Player&);
  void handlePlayerMapUpdate(net::InMessage&, Player&);

  void schedSendChunk(ChunkRef, Player&);
  void sendChunks(const std::list<ChunkRef>&, Player&);

  void chunkUpdater(WorldRef WR, bool &continueUpdate);

public:
  net::Host H;

  Server(Game &G, uint16 port);
  ~Server();
  void startInternals();
  void start();
  void run();
  void stop();
  void stopInternals();

  bool isPlayerOnline(const std::string &playername) const;
  bool isIPOnline(const std::string &ip) const;
  Player* getPlayerBySessId(uint32 id);
  Player* getPlayerByPeer(const net::Peer &peer);
  Player* getPlayerByName(const std::string &name);
  void kick(Player &p, net::QuitReason r = net::QuitReason::Kicked, const std::string& message = "");
};

}

#endif
