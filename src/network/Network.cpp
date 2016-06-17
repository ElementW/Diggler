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
	case Tfer::Unrel:
		break;
	}
	return 0;
}

Message::Message(MessageType t, uint8 s) :
	m_type(t),
	m_subtype(s),
	m_length(0),
	m_cursor(0),
	m_data(nullptr) {
}

Message::PosT Message::tell() {
	return m_cursor;
}

void Message::seek(OffT pos, Whence whence) {
	switch (whence) {
	case Begin:
		if (pos < 0) {
			m_cursor = 0;
		} else if (pos >= static_cast<OffT>(m_length)) {
			m_cursor = m_length - 1;
		} else {
			m_cursor = static_cast<PosT>(pos);
		}
		break;
	case Set:
		if (pos < -static_cast<OffT>(m_cursor)) {
			m_cursor = 0;
		} else if (pos >= static_cast<OffT>(m_length - m_cursor)) {
			m_cursor = m_length - 1;
		} else {
			m_cursor += static_cast<PosT>(pos);
		}
		break;
	}
}


InMessage::InMessage() :
	Message(MessageType::Null, 0) {
}

void InMessage::setType(MessageType type) {
	std::free(m_data);
	m_type = type;
	m_subtype = m_length = m_cursor = 0;
	m_data = nullptr;
}
void InMessage::fromData(const void *data, SizeT len) {
	// TODO: do not hardcode header size
	if (len < 2) {
		throw std::invalid_argument("Message length is smaller than message header");
	}
	const uint8 *const bytes = static_cast<const uint8*>(data);
	std::free(m_data);
	m_cursor = 0;
	m_length = len;
	m_type = static_cast<MessageType>(bytes[0]);
	m_subtype = bytes[1];
	m_data = (uint8*)std::malloc(len-2);
	std::memcpy(m_data, &(bytes[2]), len-2);
}

InMessage::~InMessage() {
	std::free(m_data);
}

OutMessage::OutMessage(MessageType t, uint8 subtype) :
	Message(t, subtype),
	m_dataMemSize(0) {
}

OutMessage::~OutMessage() {
	std::free(m_data);
}

const static int OutMessage_AllocStep = 1024;
void OutMessage::fit(SizeT len) {
	if (len <= m_dataMemSize)
		return;
	SizeT targetSize = ((len + OutMessage_AllocStep - 1) /
		OutMessage_AllocStep)*OutMessage_AllocStep; // Round up
	decltype(m_data) newData = static_cast<decltype(m_data)>(std::realloc(m_data, targetSize));
	if (newData == nullptr)
		throw std::bad_alloc();
	m_data = newData;
	m_dataMemSize = targetSize;
}

void OutMessage::writeData(const void *data, SizeT len) {
	fit(m_cursor + len);
	std::memcpy(&(m_data[m_cursor]), data, len);
	if (m_cursor + len > m_length) {
		m_length = m_cursor + len;
	}
	m_cursor += len;
}
void InMessage::readData(void *data, SizeT len) {
	if (m_cursor + len > m_length)
		throw std::underflow_error("No more data to be read");
	std::memcpy(data, &(m_data[m_cursor]), len);
	m_cursor += len;
}

void* InMessage::getCursorPtr(uint advanceCursor) {
	m_cursor += advanceCursor;
	return &(m_data[m_cursor-advanceCursor]);
}


void OutMessage::writeVec3(const glm::vec3 &vec) {
	writeFloat(vec.x);
	writeFloat(vec.y);
	writeFloat(vec.z);
}
glm::vec3 InMessage::readVec3() {
	float x, y, z;
	x = readFloat();
	y = readFloat();
	z = readFloat();
	return glm::vec3(x, y, z);
}

void OutMessage::writeIVec3(int x, int y, int z) {
	writeI32(x);
	writeI32(y);
	writeI32(z);
}
void OutMessage::writeIVec3(const glm::ivec3 &vec) {
	writeI32(vec.x);
	writeI32(vec.y);
	writeI32(vec.z);
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



Host::Host() : host(nullptr) {
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
			msg.setType(MessageType::Connect);
			break;
		case ENET_EVENT_TYPE_RECEIVE:
			peer.peer = event.peer;
			//hexDump('R', event.packet->data, event.packet->dataLength);
			msg.fromData(event.packet->data, event.packet->dataLength);
			msg.m_chan = static_cast<Channels>(event.channelID);
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

bool Host::recv(InMessage &msg, Timeout timeout) {
	ENetHost *const host = static_cast<ENetHost*>(this->host);
	ENetEvent event;
	if (enet_host_service(host, &event, timeout) >= 0){
		switch (event.type) {
		case ENET_EVENT_TYPE_NONE:
			return false;
		case ENET_EVENT_TYPE_CONNECT:
			msg.setType(MessageType::Connect);
			break;
		case ENET_EVENT_TYPE_RECEIVE:
			//hexDump('R', event.packet->data, event.packet->dataLength);
			msg.fromData(event.packet->data, event.packet->dataLength);
			msg.m_chan = static_cast<Channels>(event.channelID);
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
	ENetHost *const host = static_cast<ENetHost*>(this->host);

	uint8 header[2] = {
		static_cast<uint8>(msg.m_type),
		msg.m_subtype
	};
	ENetPacket *packet = enet_packet_create(header,
		2, TferToFlags(mode));
	enet_packet_resize(packet, 2+msg.m_length);
	std::memcpy(&packet->data[2], msg.m_data, msg.m_length);
	//hexDump('S', packet->data, 2+msg.m_length);
	enet_peer_send(static_cast<ENetPeer*>(peer.peer), static_cast<uint8>(chan), packet);
	enet_host_flush(host);
}

}
}
