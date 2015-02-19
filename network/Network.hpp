#ifndef NETWORK_HPP
#define NETWORK_HPP
#include "../Platform.hpp"
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
	MAX
};

enum class MessageType : uint8 {
	Null = 0,
	Connect,
	Disconnect,
	
	ServerInfo,
	PlayerJoin,
	PlayerUpdate,
	PlayerQuit,
	Event,
	MapTransfer,
	MapUpdate,
	StatsUpdate,
	Chat,
};

enum PlayerUpdateType : uint8 {
	Move,
	ChangeTool,
	ChangeClass,
	ChangeTeam
};

enum QuitReason : uint8 {
	Quit,
	Timeout,
	Kicked,
	Banned,
	ServerShutdown,
	UsernameAlreadyUsed
};

enum EventType : uint8 {
	ExplosivesBlow,
	PlayerJumpOnPad
};

class Message {
protected:
	friend class Host;
	MessageType m_type;
	uint8 m_subtype;
	int m_length;
	union {
		int m_cursor;
		int m_dataMemSize;
	};
	uint8 *m_data;
	
	Message() {}
	Message(const Message&) = delete;
	Message& operator=(Message&) = delete;
	Message& operator=(const Message&) = delete;

public:
	MessageType getType() const { return m_type; }
	uint8 getSubtype() const { return m_subtype; }
	int getSize() const { return m_length; }
};

class InMessage : public Message {
protected:
	friend class Host;
	void setType(MessageType type);
	void fromData(int length, void *data);

public:
	InMessage();
	~InMessage();
	
	std::string readString();
	std::u32string readString32();
	int64 readI64();
	uint64 readU64();
	int32 readI32();
	uint32 readU32();
	int16 readI16();
	uint16 readU16();
	int8 readI8();
	uint8 readU8();
	float readFloat();
	double readDouble();
	void readData(void *const data, int len);
	glm::vec3 readVec3();
};

class OutMessage : public Message {
protected:
	void fit(int);

public:
	OutMessage(MessageType t = MessageType::Null, uint8 subtype = 0);
	~OutMessage();
	
	void setType(MessageType t) { m_type = t; }
	void setSubtype(uint8 t) { m_subtype = t; }
	
	void writeString(const std::string &str);
	void writeString32(const std::u32string &str);
	void writeI64(int64 i);
	void writeU64(uint64 i);
	void writeI32(int32 i);
	void writeU32(uint32 i);
	void writeI16(int16 i);
	void writeU16(uint16 i);
	void writeI8(int8 i);
	void writeU8(uint8 i);
	void writeFloat(float f);
	void writeDouble(double d);
	void writeData(const void *const data, int len);
	void writeVec3(const glm::vec3 &vec);
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