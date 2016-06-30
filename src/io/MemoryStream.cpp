#include "MemoryStream.hpp"

#include <cstdlib>
#include <cstring>
#include <stdexcept>

namespace Diggler {

MemoryStream::MemoryStream(void *data, SizeT len) :
	m_data(static_cast<uint8*>(data)),
	m_length(len),
	m_cursor(0) {
}

void MemoryStream::seek(OffT pos, Whence whence) {
	switch (whence) {
	case Set:
		if (pos < 0) {
			m_cursor = 0;
		} else if (pos >= static_cast<OffT>(m_length)) {
			m_cursor = m_length - 1;
		} else {
			m_cursor = static_cast<PosT>(pos);
		}
		break;
	case Current:
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

InMemoryStream::InMemoryStream(const void *data, SizeT len) :
	MemoryStream(const_cast<void*>(data), len) {
	if (data == nullptr) {
		throw std::invalid_argument("data is nullptr");
	}
}

void InMemoryStream::readData(void *data, SizeT len) {
	if (m_cursor + len > m_length)
		throw std::underflow_error("No more data to be read");
	std::memcpy(data, &(m_data[m_cursor]), len);
	m_cursor += len;
}


OutMemoryStream::OutMemoryStream(SizeT prealloc) :
	MemoryStream(nullptr, 0),
	m_allocated(0) {
	if (prealloc > 0) {
		fit(prealloc);
	}
}

OutMemoryStream::~OutMemoryStream() {
	std::free(m_data);
}

const static int OutMemoryStream_AllocStep = 1024;
void OutMemoryStream::fit(SizeT len) {
	if (len <= m_allocated)
		return;
	SizeT targetSize = ((len + OutMemoryStream_AllocStep - 1) /
		OutMemoryStream_AllocStep)*OutMemoryStream_AllocStep; // Round up
	using DataT = decltype(m_data);
	DataT newData = static_cast<DataT>(
		std::realloc(m_data, targetSize));
	if (newData == nullptr)
		throw std::bad_alloc();
	m_data = newData;
	m_allocated = targetSize;
}

void OutMemoryStream::writeData(const void *data, SizeT len) {
	fit(m_cursor + len);
	std::memcpy(&(m_data[m_cursor]), data, len);
	if (m_cursor + len > m_length) {
		m_length = m_cursor + len;
	}
	m_cursor += len;
}
}
