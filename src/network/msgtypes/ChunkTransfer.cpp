#include "ChunkTransfer.hpp"

namespace Diggler {
namespace Net {
namespace MsgTypes {

void ChunkTransferRequest::writeToMsg(OutMessage &msg) const {
  msg.setType(MessageType::ChunkTransfer, ChunkTransferSubtype::Request);

  msg.writeU8(chunks.size());
  for (const ChunkData &c : chunks) {
    msg.writeI32(c.worldId);
    msg.writeIVec3(c.chunkPos);
  }
}

void ChunkTransferRequest::readFromMsg(InMessage &msg) {
  uint8 count = msg.readU8();
  chunks.clear();
  chunks.reserve(count);
  for (uint8 i  = 0; i < count; ++i) {
    chunks.emplace_back();
    ChunkData &c = chunks.back();
    c.worldId = msg.readI32();
    c.chunkPos = msg.readIVec3();
  }
}


void ChunkTransferResponse::writeToMsg(OutMessage &msg) const {
  msg.setType(MessageType::ChunkTransfer, ChunkTransferSubtype::Response);

  msg.writeU8(chunks.size());
  for (const ChunkData &c : chunks) {
    msg.writeI32(c.worldId);
    msg.writeIVec3(c.chunkPos);
    msg.writeU32(c.dataLength);
    msg.writeData(c.data, c.dataLength);
  }
}

void ChunkTransferResponse::readFromMsg(InMessage &msg) {
  uint8 count = msg.readU8();
  chunks.clear();
  chunks.reserve(count);
  for (uint8 i  = 0; i < count; ++i) {
    chunks.emplace_back();
    ChunkData &c = chunks.back();
    c.worldId = msg.readI32();
    c.chunkPos = msg.readIVec3();
    c.dataLength = msg.readU32();
    c.data = msg.getCursorPtr(c.dataLength);
  }
}


void ChunkTransferDenied::writeToMsg(OutMessage &msg) const {
  msg.setType(MessageType::ChunkTransfer, ChunkTransferSubtype::Denied);

  msg.writeU8(reason);
}

void ChunkTransferDenied::readFromMsg(InMessage &msg) {
  reason = static_cast<decltype(reason)>(msg.readU8());
}

}
}
}
