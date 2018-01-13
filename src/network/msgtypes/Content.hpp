#ifndef DIGGLER_NET_MSGTYPES_CONTENT_HPP
#define DIGGLER_NET_MSGTYPES_CONTENT_HPP

#include "MsgType.hpp"

#include <map>
#include <vector>

#include "../../content/Mod.hpp"

namespace diggler {
namespace net {
namespace MsgTypes {

enum class ContentSubtype : uint8 {
  ModListRequest,
  ModListResponse,

  ModInfoRequest,
  ModInfoResponse,

  AssetRequest,
  AssetResponse,

  FileRequest,
  FileTransfer
};


struct ContentModListRequest : public MsgType {
  void writeToMsg(OutMessage&) const override;
  void readFromMsg(InMessage&) override;
};

struct ContentModListResponse : public MsgType {
  std::vector<content::ModId> modsIds;

  void writeToMsg(OutMessage&) const override;
  void readFromMsg(InMessage&) override;
};


struct ContentModInfoRequest : public ContentModListResponse {
};

struct ContentModInfoResponse : public MsgType {
  // TODO std::map<content::ModId, > mods;

  void writeToMsg(OutMessage&) const override;
  void readFromMsg(InMessage&) override;
};

}
}
}

#endif /* DIGGLER_NET_MSGTYPES_CONTENT_HPP */
