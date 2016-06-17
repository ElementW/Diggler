#ifndef NETWORK_HPP
#define NETWORK_HPP
#include "../Platform.hpp"
#include "../io/Stream.hpp"
#include <glm/vec3.hpp>
#include <exception>

namespace Diggler {
namespace Net {

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

	PlayerJoin = 1,
	PlayerUpdate,
	PlayerQuit,
	ChunkTransfer,
	ChunkUpdate,
	StatsUpdate,
	Event,
	Chat,
  
	Connect = 240,
	Disconnect
};

enum PlayerUpdateType : uint8 {
	Move,
	ChangeTool,
	ChangeClass,
	ChangeTeam,
	Die,
	Respawn,
	ToolUse
};

enum QuitReason : uint8 {
	Quit,
	Timeout,
	Kicked,
	Banned,
	ServerShutdown,
	UsernameAlreadyUsed
};

using EventType = uint32;

class Message : public SeekableStream {
protected:
	friend class Host;
	MessageType m_type;
	uint8 m_subtype;
	SizeT m_length;
	PosT m_cursor;
	uint8 *m_data;
	
	Message() {}
	Message(MessageType, uint8);
	Message(const Message&) = delete;
	Message& operator=(Message&) = delete;
	Message& operator=(const Message&) = delete;

public:
	PosT tell() override;
	void seek(OffT, Whence = Set) override;

	MessageType getType() const { return m_type; }
	uint8 getSubtype() const { return m_subtype; }
	int getSize() const { return m_length; }
};

class InMessage : public Message, public InStream {
protected:
	friend class Host;
	Channels m_chan;
	void setType(MessageType type);
	void fromData(int length, const void *data);

public:
	InMessage();
	~InMessage();

	void readData(void *data, int len) override;
	void* getCursorPtr(uint advanceCursor = 0);

	glm::vec3 readVec3();
	glm::ivec3 readIVec3();

	Channels getChannel() const;
};

class OutMessage : public Message, public OutStream {
protected:
	int m_dataMemSize;
	void fit(int);

public:
	OutMessage(MessageType t = MessageType::Null, uint8 subtype = 0);
	~OutMessage();

	void setType(MessageType t) { m_type = t; }
	void setSubtype(uint8 t) { m_subtype = t; }

	void writeData(const void *data, int len) override;
	void writeVec3(const glm::vec3 &vec);
	void writeIVec3(const glm::vec3 &vec);
	void writeIVec3(int x, int y, int z);
};

class Exception : public std::exception {
};

struct Peer {
	void *peer;

	bool operator==(const Peer&) const;
	bool operator!=(const Peer&) const;

	void disconnect();
	std::string getHost();
	std::string getIp();
	int getPort();
};

class Host {
private:
	void *host;

	Host(const Host&) = delete;
	Host& operator=(Host&) = delete;
	Host& operator=(const Host&) = delete;

public:
	Host();
	void create(int port = -1, int maxconn = 64);
	Peer connect(const std::string &host, int port, int timeout);
	~Host();

	void send(Peer &peer, const OutMessage &msg, Tfer mode = Tfer::Rel, Channels chan = Channels::Base);
	bool recv(InMessage &msg, Peer &peer, int timeout);
	bool recv(InMessage &msg, int timeout = 0);
};

}
}

#endif
