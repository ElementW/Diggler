#ifndef NETWORK_HPP
#define NETWORK_HPP

#include <string>

#include <glm/vec3.hpp>

#include "../crypto/DiffieHellman.hpp"
#include "../io/MemoryStream.hpp"
#include "../platform/PreprocUtils.hpp"
#include "../platform/Types.hpp"

namespace meiose {
class variant;
}

namespace diggler {
namespace net {

bool Init();
void DeInit();
std::string GetNetworkLibVersion();

enum class Tfer {
  Rel,
  Unrel,
  Unseq
};

enum class Channels : uint8 {
  Base = 0,
  ConnectionMeta,
  ConnectionMetaPlain,
  Chat,
  Life,
  Movement,
  PlayerInfo,
  PlayerInteract,
  MapUpdate,
  MapTransfer,
  MAX
};

enum class MessageType : uint8 {
  Null = 0,

  ServerInfo = 220,

  ConnectionParam = 200,

  PlayerJoin = 1,
  PlayerUpdate,
  PlayerQuit,
  Content,
  ChunkTransfer,
  BlockUpdate,
  Chat,

  NetConnect = 240,
  NetDisconnect
};

enum QuitReason : uint8 {
  Quit,
  Timeout,
  Kicked,
  Banned,
  ServerShutdown
};

using EventType = uint32;

class Message : public virtual io::MemoryStream {
protected:
  friend class Host;
  MessageType m_type;
  uint8 m_subtype;

  Message() {}
  Message(MessageType, uint8);
  Message(const Message&) = delete;
  Message& operator=(Message&) = delete;
  Message& operator=(const Message&) = delete;

public:
  static constexpr uint HeaderSize = 2;

  virtual ~Message() {}

  inline MessageType getType() const { return m_type; }
  inline uint8 getSubtype() const { return m_subtype; }
  template<typename T,
           typename = std::enable_if_t<std::is_integral<std::underlying_type_t<T>>::value>>
  inline T getSubtype() const { return static_cast<T>(m_subtype); }
};

class InMessage : public Message, public io::InMemoryStream {
protected:
  friend class Host;
  Channels m_chan;
  void setType(MessageType type);
  void fromData(const void *data, SizeT, Channels);
  void free();

public:
  InMessage();
  ~InMessage();

  inline const void* getCursorPtr(uint advanceCursor = 0) {
    m_cursor += advanceCursor;
    return &(m_data[m_cursor-advanceCursor]);
  }
  inline const void* getCursorPtr() const {
    return &(m_data[m_cursor]);
  }

  glm::vec3 readVec3();
  glm::ivec3 readIVec3();
  void readMsgpack(meiose::variant&);

  Channels getChannel() const;
};

class OutMessage : public Message, public io::OutMemoryStream {
protected:
  friend class Host;
  mutable uint8 *m_actualData;
  void fit(SizeT) override;

public:
  OutMessage(MessageType t = MessageType::Null, uint8 subtype = 255);
  ~OutMessage();

  inline void setType(MessageType t) { m_type = t; }
  template<typename T>
  inline void setSubtype(T t) { m_subtype = static_cast<uint8>(t); }
  template<typename T>
  inline void setType(MessageType t, T st) { setType(t); setSubtype(st); }

  inline void writeVec3(float x, float y, float z) {
    writeFloat(x);
    writeFloat(y);
    writeFloat(z);
  }
  inline void writeVec3(const glm::vec3 &vec) {
    writeFloat(vec.x);
    writeFloat(vec.y);
    writeFloat(vec.z);
  }
  inline void writeIVec3(int x, int y, int z) {
    writeI32(x);
    writeI32(y);
    writeI32(z);
  }
  inline void writeIVec3(const glm::ivec3 &vec) {
    writeI32(vec.x);
    writeI32(vec.y);
    writeI32(vec.z);
  }
  void writeMsgpack(const meiose::variant&);
};

class Exception : public std::exception {
};

using Port = uint16;

class Host;

struct Peer {
  Crypto::DiffieHellman::SecretKey connectionSk;
  Crypto::DiffieHellman::PublicKey connectionPk;

  Crypto::DiffieHellman::PublicKey remotePk;
  Crypto::DiffieHellman::SharedSecret sharedSecret;

  Host &host;
  void *const peer;

  Peer(Host&, void*);
  nocopy(Peer);
  nomove(Peer);

  bool operator==(const Peer&) const;
  bool operator!=(const Peer&) const;

  /**
   * @brief Disconnects the peer.
   * @param data
   * Adds the peer for pending disconnection. A NetDisconnect event will be generated once the
   * peer has successfully disconnected, and the current Peer object will be deallocated as per
   * Host::recv's rules.
   */
  void disconnect(uint32 data = 0);
  std::string peerHost();
  std::string peerIP();
  Port peerPort();
};

class Host {
private:
  std::vector<Peer*> m_peersToDelete;
  void processPeersToDelete();

  void *host;
  uint64 rxBytes, txBytes;

  Host(const Host&) = delete;
  Host& operator=(Host&) = delete;
  Host& operator=(const Host&) = delete;

  void sendKeyExchange(Peer&);

public:
  using Timeout = uint32;

public:
  Host();
  ~Host();
  void create(Port port = 0, uint maxconn = 64);
  Peer& connect(const std::string &hostAddr, Port port, Timeout timeout);

  void send(Peer &peer, const OutMessage &msg, Tfer mode = Tfer::Rel, Channels chan = Channels::Base);

  // Returns true if a message is available, and put it in msg.
  // msg may be modified even if recv returns false.
  // If the msg is a NetDisconnect, returned peer object is put on a deletion list and will be
  // freed upon the next call to recv.
  bool recv(InMessage &msg, Peer **peer, Timeout timeout);
  inline bool recv(InMessage &msg, Timeout timeout) {
    return recv(msg, nullptr, timeout);
  }

  inline uint64 getRxBytes() const {
    return rxBytes;
  }
  inline uint64 getTxBytes() const {
    return txBytes;
  }
};

}
}

#endif
