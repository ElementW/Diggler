#ifndef DIGGLER_NET_MSGTYPES_CHAT_HPP
#define DIGGLER_NET_MSGTYPES_CHAT_HPP

#include "MsgType.hpp"

#include <goodform/variant.hpp>

#include "../../Player.hpp"

namespace Diggler {
namespace Net {
namespace MsgTypes {

enum class ChatSubtype : uint8 {
  Send = 0,
  Announcement,
  PlayerTalk
};

struct ChatSend : public MsgType {
  goodform::variant msg;

  void writeToMsg(OutMessage&) const override;
  void readFromMsg(InMessage&) override;
};

struct ChatAnnouncement : public MsgType {
  std::string announcementType;
  struct Origin {
    std::string name;
    goodform::variant display;
  } origin;
  goodform::variant msg;

  void writeToMsg(OutMessage&) const override;
  void readFromMsg(InMessage&) override;
};

struct ChatPlayerTalk : public MsgType {
  struct ChatPlayer {
    PlayerGameID id;
    goodform::variant display;
  } player;
  goodform::variant msg;

  void writeToMsg(OutMessage&) const override;
  void readFromMsg(InMessage&) override;
};

}
}
}

#endif /* DIGGLER_NET_MSGTYPES_CHAT_HPP */
