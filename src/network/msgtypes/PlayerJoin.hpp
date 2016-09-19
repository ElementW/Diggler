#ifndef DIGGLER_NET_MSGTYPES_PLAYER_JOIN_HPP
#define DIGGLER_NET_MSGTYPES_PLAYER_JOIN_HPP

#include "MsgType.hpp"
#include "../../Player.hpp"

namespace Diggler {
namespace Net {
namespace MsgTypes {

enum class PlayerJoinSubtype : uint8 {
  Request,
  Success,
  Failure,
  Broadcast
};

struct PlayerJoinRequest : public MsgType {
  std::string name;

  void writeToMsg(OutMessage&) const override;
  void readFromMsg(InMessage&) override;
};

struct PlayerJoinSuccess : public MsgType {
  Player::SessionID sessId;
  WorldId wid;

  void writeToMsg(OutMessage&) const override;
  void readFromMsg(InMessage&) override;
};

struct PlayerJoinFailure : public MsgType {
  enum FailureReason : uint8 {
    Unknown,
    Banned,
    ServerShutdown,
    UsernameAlreadyUsed
  } failureReason = Unknown;

  void writeToMsg(OutMessage&) const override;
  void readFromMsg(InMessage&) override;
};

struct PlayerJoinBroadcast : public MsgType {
  enum JoinReason : uint8 {
    JoiningGame,
    AlreadyPlaying
  } joinReason = JoiningGame;
  std::string name;
  Player::SessionID sessId;
  WorldId wid;

  void writeToMsg(OutMessage&) const override;
  void readFromMsg(InMessage&) override;
};

}
}
}

#endif /* DIGGLER_NET_MSGTYPES_PLAYER_JOIN_HPP */
