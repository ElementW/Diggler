#ifndef DIGGLER_NET_MSGTYPES_PLAYER_UPDATE_HPP
#define DIGGLER_NET_MSGTYPES_PLAYER_UPDATE_HPP

#include <optional.hpp>

#include "MsgType.hpp"
#include "../../Player.hpp" // for DeathReason

namespace diggler {
namespace net {
namespace MsgTypes {

enum class PlayerUpdateSubtype : uint8 {
  Move,
  Die,
  Respawn,
  ChangeTool,
  ToolUse
};

struct PlayerUpdateMove : public MsgType {
  std::experimental::optional<Player::SessionID> plrSessId;
  std::experimental::optional<glm::vec3> position, velocity, accel;
  float angle;

  void writeToMsg(OutMessage&) const override;
  void readFromMsg(InMessage&) override;
};

struct PlayerUpdateDie : public MsgType {
  Player::SessionID plrSessId;

  void writeToMsg(OutMessage&) const override;
  void readFromMsg(InMessage&) override;
};

struct PlayerUpdateRespawn : public MsgType {
  Player::SessionID plrSessId;

  void writeToMsg(OutMessage&) const override;
  void readFromMsg(InMessage&) override;
};

}
}
}

#endif /* DIGGLER_NET_MSGTYPES_PLAYER_UPDATE_HPP */
