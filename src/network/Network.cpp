#include "Network.hpp"
#include <enet/enet.h>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <memory>

#include <iomanip>

namespace Diggler {
namespace Net {

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
  m_chan(Channels::Base),
  m_packet(nullptr) {
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
  m_length = len;
  m_type = static_cast<MessageType>(bytes[0]);
  m_subtype = bytes[1];
  m_data = static_cast<uint8*>(std::malloc(len-HeaderSize));
  std::memcpy(m_data, &(bytes[HeaderSize]), len-HeaderSize);
}

void InMessage::fromPacket(void *packet, Channels chan) {
  ENetPacket *enpkt = static_cast<ENetPacket*>(packet);
  SizeT len = enpkt->dataLength;
  if (len < HeaderSize) {
    throw std::invalid_argument("Message length is smaller than message header");
  }
  uint8 *const bytes = static_cast<uint8*>(enpkt->data);
  free();
  m_packet = packet;
  m_chan = chan;
  m_cursor = 0;
  m_length = len;
  m_type = static_cast<MessageType>(bytes[0]);
  m_subtype = bytes[1];
  m_data = &(bytes[HeaderSize]);
}

void InMessage::free() {
  if (m_packet != nullptr) {
    enet_packet_destroy(static_cast<ENetPacket*>(m_packet));
  } else {
    std::free(m_data);
  }
  m_type = MessageType::Null;
  m_subtype = m_length = m_cursor = 0;
  m_packet = m_data = nullptr;
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


Channels InMessage::getChannel() const {
  return m_chan;
}

bool Peer::operator==(const Peer &other) const {
  return peer == other.peer;
}

bool Peer::operator!=(const Peer &other) const {
  return !(*this == other);
}

void Peer::disconnect() {
  ENetPeer *const peer = static_cast<ENetPeer*>(this->peer);
  enet_peer_disconnect(peer, 0);
}

std::string Peer::getHost() {
  const ENetPeer *const peer = static_cast<const ENetPeer*>(this->peer);
  std::ostringstream oss;
  char *chars = new char[512];
  enet_address_get_host_ip(&peer->host->address, chars, 512);
  oss << chars;
  delete[] chars;
  oss << ':' << peer->host->address.port;
  return oss.str();
}

std::string Peer::getIp() {
  const ENetPeer *const peer = static_cast<const ENetPeer*>(this->peer);
  char *chars = new char[512];
  enet_address_get_host_ip(&peer->host->address, chars, 512);
  std::string str(chars);
  delete[] chars;
  return str;
}

Port Peer::getPort() {
  const ENetPeer *const peer = static_cast<const ENetPeer*>(this->peer);
  return peer->host->address.port;
}



Host::Host() :
  host(nullptr),
  rxBytes(0),
  txBytes(0) {
}

void Host::create(Port port, uint maxconn) {
  if (port == 0) { // Client
    host = enet_host_create(nullptr, 1, static_cast<size_t>(Channels::MAX), 0, 0);
  } else { // Server
    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = port;
    host = enet_host_create(&address, maxconn, static_cast<size_t>(Channels::MAX), 0, 0);
  }
  if (host == nullptr) {
    throw Exception();
  }
}

Peer Host::connect(const std::string &hostAddr, Port port, Timeout timeout) {
  ENetHost *const host = static_cast<ENetHost*>(this->host);
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
    Peer p; p.peer = peer;
    return p;
  }

  enet_peer_reset(peer);
  throw Exception();
}

Host::~Host() {
  ENetHost *const host = static_cast<ENetHost*>(this->host);
  enet_host_destroy(host);
}

/*static void hexDump(char in, uint8 *buf, int len) {
  std::cout << in << ": " << std::setiosflags(std::ios::internal);
  for (int i=0; i < len; ++i) {
    std::cout << std::setfill('0') << std::setw(2) << std::hex << (int)buf[i] << ' ';
  }
  std::cout << std::dec << std::endl;
}*/

bool Host::recv(InMessage &msg, Peer &peer, Timeout timeout) {
  ENetHost *const host = static_cast<ENetHost*>(this->host);
  ENetEvent event;
  if (enet_host_service(host, &event, timeout) >= 0){
    switch (event.type) {
    case ENET_EVENT_TYPE_NONE:
      return false;
    case ENET_EVENT_TYPE_CONNECT:
      peer.peer = event.peer;
      msg.setType(MessageType::NetConnect);
      break;
    case ENET_EVENT_TYPE_RECEIVE:
      peer.peer = event.peer;
      //hexDump('R', event.packet->data, event.packet->dataLength);
      // Packet "ownership" is transferred to msg
      msg.fromPacket(event.packet, static_cast<Channels>(event.channelID));
      rxBytes += event.packet->dataLength;
      break;
    case ENET_EVENT_TYPE_DISCONNECT:
      peer.peer = event.peer;
      msg.setType(MessageType::NetDisconnect);
      enet_peer_reset(event.peer);
    }
    return true;
  }
  throw Exception();
}

bool Host::recv(InMessage &msg, Timeout timeout) {
  ENetHost *const host = static_cast<ENetHost*>(this->host);
  ENetEvent event;
  if (enet_host_service(host, &event, timeout) >= 0){
    switch (event.type) {
    case ENET_EVENT_TYPE_NONE:
      return false;
    case ENET_EVENT_TYPE_CONNECT:
      msg.setType(MessageType::NetConnect);
      break;
    case ENET_EVENT_TYPE_RECEIVE:
      //hexDump('R', event.packet->data, event.packet->dataLength);
      // Packet "ownership" is transferred to msg
      msg.fromPacket(event.packet, static_cast<Channels>(event.channelID));
      rxBytes += event.packet->dataLength;
      break;
    case ENET_EVENT_TYPE_DISCONNECT:
      msg.setType(MessageType::NetDisconnect);
      enet_peer_reset(event.peer);
    }
    return true;
  }
  throw Exception();
}

void Host::send(Peer &peer, const OutMessage &msg, Tfer mode, Channels chan) {
  ENetHost *const host = static_cast<ENetHost*>(this->host);

  const uint8 header[Message::HeaderSize] = {
    static_cast<uint8>(msg.m_type),
    msg.m_subtype
  };

  ENetPacket *packet;
  if (msg.m_actualData != nullptr) {
    std::memcpy(msg.m_actualData, header, Message::HeaderSize);
    packet = enet_packet_create(msg.m_actualData,
      Message::HeaderSize + msg.m_length, TferToFlags(mode));
    txBytes += Message::HeaderSize + msg.m_length;
  } else {
    packet = enet_packet_create(header,
      Message::HeaderSize, TferToFlags(mode));
    txBytes += Message::HeaderSize;
  }

  //hexDump('S', packet->data, 2+msg.m_length);
  enet_peer_send(static_cast<ENetPeer*>(peer.peer), static_cast<uint8>(chan), packet);
  enet_host_flush(host);
}

}
}
