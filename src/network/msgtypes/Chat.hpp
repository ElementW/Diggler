#ifndef DIGGLER_NET_MSGTYPES_CHAT_HPP
#define DIGGLER_NET_MSGTYPES_CHAT_HPP

#include "MsgType.hpp"

#include <meiose/variant.hpp>

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
  meiose::variant msg;

  void writeToMsg(OutMessage&) const override;
  void readFromMsg(InMessage&) override;
};

struct ChatAnnouncement : public MsgType {
  std::string announcementType;
  struct Origin {
    std::string name;
    meiose::variant display;
  } origin;
  meiose::variant msg;

  void writeToMsg(OutMessage&) const override;
  void readFromMsg(InMessage&) override;
};

struct ChatPlayerTalk : public MsgType {
  struct ChatPlayer {
    PlayerGameID id;
    meiose::variant display;
  } player;
  meiose::variant msg;

  void writeToMsg(OutMessage&) const override;
  void readFromMsg(InMessage&) override;
};

}
}
}

#endif /* DIGGLER_NET_MSGTYPES_CHAT_HPP */
