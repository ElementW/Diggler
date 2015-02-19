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
	sstm << "ENet v" << ENET_VERSION_MAJOR << '.' << ENET_VERSION_MINOR
		<< '.' << ENET_VERSION_PATCH << '(' << ENET_VERSION_GET_MAJOR(ver)
		<< '.' << ENET_VERSION_GET_MINOR(ver) << '.' << ENET_VERSION_GET_PATCH(ver)
		<< ')';
	return sstm.str();
}

static enet_uint32 TferToFlags(Tfer mode) {
	switch (mode) {
		case Tfer::Rel:
			return ENET_PACKET_FLAG_RELIABLE;
		case Tfer::Unseq:
			return ENET_PACKET_FLAG_UNSEQUENCED;
		//case Tfer::Unrel:
			//return 0;
		default:
			break;
	}
	return 0;
}

InMessage::InMessage() {
	m_type = MessageType::Null;
	m_subtype = m_length = m_cursor = 0;
	m_data = nullptr;
}

void InMessage::setType(MessageType type) {
	std::free(m_data);
	m_type = type;
	m_subtype = m_length = m_cursor = 0;
	m_data = nullptr;
}
void InMessage::fromData(int length, void* data) {
	std::free(m_data);
	m_cursor = 0;
	m_length = length;
	m_type = (MessageType)((uint8*)data)[0];
	m_subtype = ((uint8*)data)[1];
	m_data = (uint8*)std::malloc(length-2);
	std::memcpy(m_data, &(((uint8*)data)[2]), length-2);
}

InMessage::~InMessage() {
	std::free(m_data);
}

OutMessage::OutMessage(MessageType t, uint8 subtype) {
	m_type = t;
	m_subtype = subtype;
	m_length = m_dataMemSize = 0;
	m_data = nullptr;
}

OutMessage::~OutMessage() {
	std::free(m_data);
}

const static int OutMessage_AllocStep = 1024;
void OutMessage::fit(int len) {
	if (len <= m_dataMemSize)
		return;
	int targetSize = ((len + OutMessage_AllocStep - 1) / OutMessage_AllocStep)*OutMessage_AllocStep; // Round up
	m_data = (uint8*)std::realloc(m_data, targetSize);
	// TODO: handle failure?
	m_dataMemSize = targetSize;
}

void OutMessage::writeData(const void *const data, int len) {
	fit(m_length + len);
	std::memcpy(&(m_data[m_length]), data, len);
	m_length += len;
}
void InMessage::readData(void *const data, int len) {
	if (m_cursor + len > m_length)
		throw std::underflow_error("No more data to be read");
	std::memcpy(data, &(m_data[m_cursor]), len);
	m_cursor += len;
}
void OutMessage::writeString(const std::string &str) {
	if (str.size() > UINT16_MAX)
		throw std::length_error("String too long");
	int len = str.size();
	writeU16(len);
	fit(m_length + len);
	std::memcpy(&(m_data[m_length]), str.c_str(), len);
	m_length += len;
}
std::string InMessage::readString() {
	uint16 len = readU16();
	m_cursor += len;
	return std::string((char*)&(m_data[m_cursor-len]), len);
}
void OutMessage::writeString32(const std::u32string &str) {
	if (str.size() > UINT16_MAX)
		throw std::length_error("String too long");
	int len = str.size();
	writeU16(len);
	fit(m_length + len*4);
	std::memcpy(&(m_data[m_length]), str.c_str(), len*4);
	m_length += len*4;
}
std::u32string InMessage::readString32() {
	uint16 len = readU16();
	m_cursor += len*4;
	return std::u32string((char32_t*)&(m_data[m_cursor-len*4]), len);
}
void OutMessage::writeVec3(const glm::vec3 &vec) {
	writeFloat(vec.x);
	writeFloat(vec.y);
	writeFloat(vec.z);
}
glm::vec3 InMessage::readVec3() {
	struct { float x, y, z; } data;
	data.x = readFloat();
	data.y = readFloat();
	data.z = readFloat();
	return glm::vec3(data.x, data.y, data.z);
}
void OutMessage::writeI64(int64 i) {
	writeData(&i, 8);
}
int64 InMessage::readI64() {
	int64 val;
	readData(&val, 8);
	return val;
}
void OutMessage::writeU64(uint64 i) {
	writeData(&i, 8);
}
uint64 InMessage::readU64() {
	uint64 val;
	readData(&val, 8);
	return val;
}
void OutMessage::writeI32(int32 i) {
	writeData(&i, 4);
}
int32 InMessage::readI32() {
	int32 val;
	readData(&val, 4);
	return val;
}
void OutMessage::writeU32(uint32 i) {
	writeData(&i, 4);
}
uint32 InMessage::readU32() {
	uint32 val;
	readData(&val, 4);
	return val;
}
void OutMessage::writeI16(int16 i) {
	writeData(&i, 2);
}
int16 InMessage::readI16() {
	int16 val;
	readData(&val, 2);
	return val;
}
void OutMessage::writeU16(uint16 i) {
	writeData(&i, 2);
}
uint16 InMessage::readU16() {
	uint16 val;
	readData(&val, 2);
	return val;
}
void OutMessage::writeI8(int8 i) {
	writeData(&i, 1);
}
int8 InMessage::readI8() {
	int8 val;
	readData(&val, 1);
	return val;
}
void OutMessage::writeU8(uint8 i) {
	writeData(&i, 1);
}
uint8 InMessage::readU8() {
	uint8 val;
	readData(&val, 1);
	return val;
}
void OutMessage::writeFloat(float f) {
	writeData(&f, sizeof(float));
}
float InMessage::readFloat() {
	float val;
	readData(&val, sizeof(float));
	return val;
}
void OutMessage::writeDouble(double d) {
	writeData(&d, sizeof(double));
}
double InMessage::readDouble() {
	double val;
	readData(&val, sizeof(double));
	return val;
}



bool Peer::operator==(const Peer &other) const {
	return peer == other.peer;
}

bool Peer::operator!=(const Peer &other) const {
	return !(*this == other);
}

void Peer::disconnect() {
	enet_peer_disconnect((ENetPeer*)peer, 0);
}

std::string Peer::getHost() {
	std::ostringstream oss;
	char *chars = new char[512];
	enet_address_get_host_ip(&((ENetPeer*)peer)->host->address, chars, 512);
	oss << chars;
	delete[] chars;
	oss << ':' << ((ENetPeer*)peer)->host->address.port;
	return oss.str();
}

std::string Peer::getIp() {
	char *chars = new char[512];
	enet_address_get_host_ip(&((ENetPeer*)peer)->host->address, chars, 512);
	std::string str(chars);
	delete[] chars;
	return str;
}

int Peer::getPort() {
	return ((ENetPeer*)peer)->host->address.port;
}



Host::Host() : host(nullptr) {
}

void Host::create(int port, int maxconn) {
	if (port == -1) { // Client
		host = enet_host_create(nullptr, 1, (uint8)Channels::MAX, 0, 0);
	} else { // Server
		ENetAddress address;
		address.host = ENET_HOST_ANY;
		address.port = port;
		host = enet_host_create(&address, maxconn, (uint8)Channels::MAX, 0, 0);
	}
	if (host == nullptr) {
		throw Exception();
	}
}

Peer Host::connect(const std::string &host, int port, int timeout) {
	ENetAddress address;
	ENetEvent event;
	ENetPeer *peer;
	
	enet_address_set_host(&address, host.c_str());
	address.port = port;
	
	peer = enet_host_connect((ENetHost*)this->host, &address, 1, 0);
	if (peer == nullptr) {
		throw Exception();
	}
	
	if (enet_host_service((ENetHost*)this->host, &event, timeout) > 0 &&
		event.type == ENET_EVENT_TYPE_CONNECT) {
		Peer p; p.peer = peer;
		return p;
	}
	enet_peer_reset(peer);
	throw Exception();
}

Host::~Host() {
	enet_host_destroy((ENetHost*)host);
}

/*static void hexDump(char in, uint8 *buf, int len) {
	std::cout << in << ": " << std::setiosflags(std::ios::internal);
	for (int i=0; i < len; ++i) {
		std::cout << std::setfill('0') << std::setw(2) << std::hex << (int)buf[i] << ' ';
	}
	std::cout << std::dec << std::endl;
}*/

bool Host::recv(InMessage &msg, Peer &peer, int timeout) {
	ENetEvent event;
	if (enet_host_service((ENetHost*)host, &event, timeout) >= 0){
		switch (event.type) {
			case ENET_EVENT_TYPE_NONE:
				return false;
			case ENET_EVENT_TYPE_CONNECT:
				peer.peer = event.peer;
				msg.setType(MessageType::Connect);
				break;
			case ENET_EVENT_TYPE_RECEIVE:
				peer.peer = event.peer;
				//hexDump('R', event.packet->data, event.packet->dataLength);
				msg.fromData(event.packet->dataLength, event.packet->data);
				enet_packet_destroy(event.packet);
				break;
			case ENET_EVENT_TYPE_DISCONNECT:
				peer.peer = event.peer;
				msg.setType(MessageType::Disconnect);
				enet_peer_reset(event.peer);
		}
		return true;
	}
	throw Exception();
}

bool Host::recv(InMessage &msg, int timeout) {
	ENetEvent event;
	if (enet_host_service((ENetHost*)host, &event, timeout) >= 0){
		switch (event.type) {
			case ENET_EVENT_TYPE_NONE:
				return false;
			case ENET_EVENT_TYPE_CONNECT:
				msg.setType(MessageType::Connect);
				break;
			case ENET_EVENT_TYPE_RECEIVE:
				//hexDump('R', event.packet->data, event.packet->dataLength);
				msg.fromData(event.packet->dataLength, event.packet->data);
				enet_packet_destroy(event.packet);
				break;
			case ENET_EVENT_TYPE_DISCONNECT:
				msg.setType(MessageType::Disconnect);
				enet_peer_reset(event.peer);
		}
		return true;
	}
	throw Exception();
}

void Host::send(Peer &peer, const OutMessage &msg, Tfer mode, Channels chan) {
	uint8 header[2]; header[0] = (uint8)msg.m_type; header[1] = msg.m_subtype;
	ENetPacket *packet = enet_packet_create(header,
		2, TferToFlags(mode));
	enet_packet_resize(packet, 2+msg.m_length);
	std::memcpy(&packet->data[2], msg.m_data, msg.m_length);
	//hexDump('S', packet->data, 2+msg.m_length);
	enet_peer_send((ENetPeer*)peer.peer, (uint8)chan, packet);
	enet_host_flush((ENetHost*)host);
}

}
}