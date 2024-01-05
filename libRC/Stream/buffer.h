#ifndef BUFFER_H
#define BUFFER_H

#include <cstddef>
#include <cstring>
#include <string>
#include <type_traits>

#include "Stream/netstream.h"
#include "interface.h"

class Buffer {
	byte* data;
	size_t len            = 0;
	const size_t capacity = 0;
	const bool owned      = true;

	bool good      = true;
	size_t readPos = 0;

	void append(const void* src, size_t size);

public:
	Buffer(size_t capacity = MESSAGE_BUFLEN);

	Buffer(byte* buf, size_t capacity);

	~Buffer();

	size_t getSize() const { return len; }

	const byte* getBuffer() const { return data; }

	operator bool() const { return good; }

	void clear() { good = true; }

	void reset() { readPos = 0; }

	template <typename T>
	Buffer& operator<<(const T& in) {
		if (good) {
			if constexpr (std::is_same_v<T, std::string>) {
				append(in.c_str(), in.length() + 1);
			} else {
				append(&in, sizeof(T));
			}
		}
		return *this;
	}

	template <typename T>
	Buffer& operator>>(T& out) {
		if (readPos + sizeof(T) >= len) {
			good = false;
		}
		if (good) {
			if constexpr (std::is_same_v<T, std::string>) {
				out = std::string((char*)(data + readPos));
				readPos += out.size + 1;
			} else {
				memcpy(&out, data + readPos, sizeof(T));
				readPos += sizeof(T);
			}
		}
		return *this;
	}
};

#endif
