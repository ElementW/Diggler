#include "Network.hpp"

#include <chrono>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <memory>

#include <enet/enet.h>

#include <goodform/msgpack.hpp>

#include "../crypto/Random.hpp"
#include "../util/Log.hpp"
#include "msgtypes/ConnectionParam.hpp"

#include <iomanip>

namespace Diggler {
namespace Net {

using Util::Log;
using namespace Util::Logging::LogLevels;

static const char *TAG = "Network";

static bool InitDone = false;

bool Init() {
  if (InitDone)
    return true;
  InitDone = true;
  return enet_initialize() == 0;
}

void DeInit() {
  enet_deinitialize();
}

std::string GetNetworkLibVersion() {
  ENetVersion ver = enet_linked_version();
  std::ostringstream sstm;
  sstm << "ENet linked " << ENET_VERSION_MAJOR << '.' << ENET_VERSION_MINOR
    << '.' << ENET_VERSION_PATCH << ", using " << ENET_VERSION_GET_MAJOR(ver)
    << '.' << ENET_VERSION_GET_MINOR(ver) << '.' << ENET_VERSION_GET_PATCH(ver);
  return sstm.str();
}

static enet_uint32 TferToFlags(Tfer mode) {
  switch (mode) {
  case Tfer::Rel:
    return ENET_PACKET_FLAG_RELIABLE;
  case Tfer::Unseq:
    return ENET_PACKET_FLAG_UNSEQUENCED;
  case Tfer::Unrel:
    break;
  }
  return 0;
}

Message::Message(MessageType t, uint8 s) :
  MemoryStream(nullptr, 0),
  m_type(t),
  m_subtype(s) {
}


InMessage::InMessage() :
  Message(MessageType::Null, 0),
  m_chan(Channels::Base) {
}

InMessage::~InMessage() {
  free();
}

void InMessage::setType(MessageType type) {
  free();
  m_type = type;
  m_subtype = m_length = m_cursor = 0;
  m_data = nullptr;
}

void InMessage::fromData(const void *data, SizeT len, Channels chan) {
  if (len < HeaderSize) {
    throw std::invalid_argument("Message length is smaller than message header");
  }
  const uint8 *const bytes = static_cast<const uint8*>(data);
  free();
  m_chan = chan;
  m_cursor = 0;
  m_length = len - HeaderSize;
  m_type = static_cast<MessageType>(bytes[0]);
  m_subtype = bytes[1];
  // m_data/bytes is guaranteed never to be written to, so we can const_cast it
  m_data = const_cast<uint8*>(bytes) + HeaderSize;
}

void InMessage::free() {
  if (m_data != nullptr) {
    delete[] (m_data - HeaderSize);
  }
  m_type = MessageType::Null;
  m_subtype = m_length = m_cursor = 0;
  m_data = nullptr;
}

void InMessage::readMsgpack(goodform::variant &var) {
  uint32 len = readU32();
  if (len > remaining()) {
    throw std::runtime_error("Not enough bytes available for reported msgpack length");
  }
  InMemoryStream ims(getCursorPtr(len), len);
  goodform::msgpack::deserialize(ims, var);
  /*if (var.type() != goodform::variant_type::object) {
    throw std::runtime_error("Read msgpack is not an object / map");
  }*/
}


Channels InMessage::getChannel() const {
  return m_chan;
}


OutMessage::OutMessage(MessageType t, uint8 subtype) :
  Message(t, subtype),
  m_actualData(nullptr) {
}

OutMessage::~OutMessage() {
  std::free(m_actualData);
  m_data = nullptr;
}

const static int OutMessage_AllocStep = 1024;
void OutMessage::fit(SizeT len) {
  if (len <= m_allocated)
    return;
  SizeT targetSize = ((len + OutMessage_AllocStep - 1) /
    OutMessage_AllocStep)*OutMessage_AllocStep; // Round up
  using DataT = decltype(m_actualData);
  DataT newActualData = static_cast<DataT>(
    std::realloc(m_actualData, HeaderSize + targetSize));
  if (newActualData == nullptr)
    throw std::bad_alloc();
  m_actualData = newActualData;
  m_data = newActualData + HeaderSize;
  m_allocated = targetSize;
}

void OutMessage::writeMsgpack(const goodform::variant &var) {
  /*if (var.type() != goodform::variant_type::object) {
    throw std::runtime_error("msgpack to write is not an object / map");
  }*/
  PosT pos = tell();
  writeU32(0);
  goodform::msgpack::serialize(var, *this);
  PosT posWritten = tell();
  seek(pos);
  writeU32(static_cast<uint32>(posWritten - (pos + sizeof(uint32))));
  seek(posWritten);
}


glm::vec3 InMessage::readVec3() {
  float x, y, z;
  x = readFloat();
  y = readFloat();
  z = readFloat();
  return glm::vec3(x, y, z);
}

glm::ivec3 InMessage::readIVec3() {
  int32 x, y, z;
  x = readI32();
  y = readI32();
  z = readI32();
  return glm::ivec3(x, y, z);
}


Peer::Peer(Host &host, void *peer) :
  host(host),
  peer(peer) {
  reinterpret_cast<ENetPeer*>(this->peer)->data = this;
  Crypto::Random::randomData(connectionPk);
  Crypto::DiffieHellman::scalarmultBase(connectionSk, connectionPk);
}

bool Peer::operator==(const Peer &other) const {
  return peer == other.peer;
}

bool Peer::operator!=(const Peer &other) const {
  return !(*this == other);
}

void Peer::disconnect(uint32 data) {
  ENetPeer *const peer = reinterpret_cast<ENetPeer*>(this->peer);
  enet_peer_disconnect(peer, data);
}

std::string Peer::peerHost() {
  const ENetPeer *const peer = reinterpret_cast<const ENetPeer*>(this->peer);
  std::ostringstream oss;
  char *chars = new char[512];
  enet_address_get_host_ip(&peer->host->address, chars, 512);
  oss << chars;
  delete[] chars;
  oss << ':' << peer->host->address.port;
  return oss.str();
}

std::string Peer::peerIP() {
  const ENetPeer *const peer = reinterpret_cast<const ENetPeer*>(this->peer);
  char *chars = new char[512];
  enet_address_get_host_ip(&peer->host->address, chars, 512);
  std::string str(chars);
  delete[] chars;
  return str;
}

Port Peer::peerPort() {
  const ENetPeer *const peer = reinterpret_cast<const ENetPeer*>(this->peer);
  return peer->host->address.port;
}



Host::Host() :
  host(nullptr),
  rxBytes(0),
  txBytes(0) {
}

Host::~Host() {
  ENetHost *const host = reinterpret_cast<ENetHost*>(this->host);
  enet_host_destroy(host);
}

void Host::create(Port port, uint maxconn) {
  if (port == 0) { // Client
    host = enet_host_create(nullptr, 1, static_cast<size_t>(Channels::MAX), 0, 0);
  } else { // Server
    ENetAddress address;
    address.host = IN6ADDR_ANY_INIT; // ENET_HOST_ANY;
    address.port = port;
    host = enet_host_create(&address, maxconn, static_cast<size_t>(Channels::MAX), 0, 0);
  }
  if (host == nullptr) {
    throw Exception();
  }
}

Peer& Host::connect(const std::string &hostAddr, Port port, Timeout timeout) {
  ENetHost *const host = reinterpret_cast<ENetHost*>(this->host);
  ENetAddress address;
  ENetEvent event;
  ENetPeer *peer;

  enet_address_set_host(&address, hostAddr.c_str());
  address.port = port;

  peer = enet_host_connect(host, &address, static_cast<size_t>(Channels::MAX), 0);
  if (peer == nullptr) {
    throw Exception();
  }

  if (enet_host_service(host, &event, timeout) > 0 &&
    event.type == ENET_EVENT_TYPE_CONNECT) {
    Peer *p = new Peer(*this, peer);
    sendKeyExchange(*p);
    return *p;
  }

  enet_peer_reset(peer);
  throw Exception();
}

void Host::processPeersToDelete() {
  for (Peer *peer : m_peersToDelete) {
    delete peer;
  }
  m_peersToDelete.clear();
}

void Host::sendKeyExchange(Peer &p) {
  MsgTypes::ConnectionParamDHKeyExchange dhke;
  dhke.pk = p.connectionPk;
  OutMessage keMsg;
  dhke.writeToMsg(keMsg);
  send(p, keMsg);
}

/*static void hexDump(char in, uint8 *buf, int len) {
  std::cout << in << ": " << std::setiosflags(std::ios::internal);
  for (int i=0; i < len; ++i) {
    std::cout << std::setfill('0') << std::setw(2) << std::hex << (int)buf[i] << ' ';
  }
  std::cout << std::dec << std::endl;
}*/

bool Host::recv(InMessage &msg, Peer **peer, Timeout timeout) {
  ENetHost *const host = reinterpret_cast<ENetHost*>(this->host);
  processPeersToDelete();
  auto start = std::chrono::steady_clock::now();

  ENetEvent event;
  while (true) {
    auto now = std::chrono::steady_clock::now();
    enet_uint32 elapsed = static_cast<enet_uint32>(
      std::chrono::duration_cast<std::chrono::milliseconds>(now - start).count());
    if (enet_host_service(host, &event, timeout - elapsed) > 0) {
      Peer *peerPtr = event.peer == nullptr ? nullptr :
        reinterpret_cast<Peer*>(event.peer->data);
      switch (event.type) {
      case ENET_EVENT_TYPE_NONE:
        break;
      case ENET_EVENT_TYPE_CONNECT:
        peerPtr = new Peer(*this, event.peer);
        *peer = peerPtr;
        sendKeyExchange(*peerPtr);
        msg.setType(MessageType::NetConnect);
        return true;
      case ENET_EVENT_TYPE_RECEIVE: {
        if (peer) {
          *peer = peerPtr;
        }

        const Message::SizeT pktLen = event.packet->dataLength;
        const Channels pktChannel = static_cast<Channels>(event.channelID);
        const bool decrypt = (pktChannel == Channels::ConnectionMetaPlain);
        byte *rcvData = new uint8[pktLen];
        if (decrypt) {
          // TODO: decryption
          std::memcpy(rcvData, event.packet->data, pktLen);
        } else {
          std::memcpy(rcvData, event.packet->data, pktLen);
        }
        // pktData's ownership is transferred to msg
        msg.fromData(rcvData, pktLen, pktChannel);
        rxBytes += event.packet->dataLength;

        using CPS = MsgTypes::ConnectionParamSubtype;
        if (msg.getType() == MessageType::ConnectionParam &&
            msg.getSubtype<CPS>() == CPS::DHKeyExchange) {
          MsgTypes::ConnectionParamDHKeyExchange dhke; dhke.readFromMsg(msg);
          peerPtr->remotePk = dhke.pk;
          if (Crypto::DiffieHellman::scalarmult(peerPtr->connectionSk, peerPtr->remotePk,
            peerPtr->sharedSecret) != 0) {
            // TODO: properly handle key exchange failure
            throw std::runtime_error("DH key exchange failed");
          }
          Log(Debug, TAG) << "hello DH! " << peerPtr->sharedSecret.hex();
        } else {
          return true;
        }
      } break;
      case ENET_EVENT_TYPE_DISCONNECT:
        if (peer) {
          *peer = peerPtr;
        }
        msg.setType(MessageType::NetDisconnect);
        m_peersToDelete.emplace_back(peerPtr);
        return true;
      }
    } else {
      return false;
    }
  }
  throw Exception();
}

void Host::send(Peer &peer, const OutMessage &msg, Tfer mode, Channels chan) {
  ENetHost *const host = reinterpret_cast<ENetHost*>(this->host);
  const bool encrypt = (chan == Channels::ConnectionMetaPlain);

  const byte header[Message::HeaderSize] = {
    static_cast<byte>(msg.m_type),
    msg.m_subtype
  };

  size_t pktLen = Message::HeaderSize + (msg.m_actualData == nullptr ? 0 : msg.m_length);
  ENetPacket *packet = enet_packet_create(nullptr, pktLen, TferToFlags(mode));
  byte *pktData = packet->data;
  txBytes += pktLen;
  if (msg.m_actualData != nullptr) {
    std::memcpy(msg.m_actualData, header, Message::HeaderSize);
    if (encrypt) {
      // TODO: don't memcpy, encrypt!
      std::memcpy(pktData, msg.m_actualData, pktLen);
    } else {
      std::memcpy(pktData, msg.m_actualData, pktLen);
    }
  } else {
    if (encrypt) {
      // TODO: don't memcpy, encrypt!
      std::memcpy(pktData, header, pktLen);
    } else {
      std::memcpy(pktData, header, pktLen);
    }
  }

  //hexDump('S', pktData, pktLen);
  enet_peer_send(reinterpret_cast<ENetPeer*>(peer.peer), static_cast<uint8>(chan), packet);
  enet_host_flush(host);
}

}
}
