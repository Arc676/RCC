#ifndef BUFFER_H
#define BUFFER_H

#include <cstddef>
#include <cstring>
#include <string>
#include <type_traits>

#include "Stream/netstream.h"
#include "interface.h"

template <typename T>
concept IsByte = sizeof(T) == 1;

template <typename T>
	requires IsByte<T>
class Buffer;

template <typename T>
concept Serializable = requires(const T x) {
	{ x.serialize(*(Buffer<byte>*)0) };
};

template <typename T>
concept Deserializable = requires(T x) {
	{ x.deserialize(*(Buffer<const byte>*)0) };
};

template <typename Data>
	requires IsByte<Data>
class Buffer {
	Data* data;
	size_t len            = 0;
	const size_t capacity = 0;
	const bool owned      = true;

	bool good      = true;
	size_t readPos = 0;

	void append(const void* src, size_t size)
		requires(!std::is_const_v<Data>);

public:
	Buffer(size_t capacity = MESSAGE_BUFLEN);

	Buffer(Data* buf, size_t size);

	~Buffer();

	size_t size() const { return len; }

	const Data* getBuffer() const { return data; }

	Data peek() const { return data[readPos]; }

	bool ok() const { return good; }

	void reset() { good = true; }

	void clear() {
		len = 0;
		reset();
	}

	size_t tell() const { return readPos; }

	void rewind() { readPos = 0; }

	template <typename T>
	Buffer& operator<<(const T& in)
		requires(!std::is_const_v<Data>)
	{
		if (good) {
			if constexpr (Serializable<T>) {
				in.serialize(*this);
			} else if constexpr (std::is_same_v<T, std::string>) {
				append(in.c_str(), in.length() + 1);
			} else {
				append(&in, sizeof(T));
			}
		}
		return *this;
	}

	template <typename T>
	Buffer& operator>>(T& out) {
		if constexpr (Deserializable<T>) {
			out.deserialize(*this);
			return *this;
		}
		if (readPos + sizeof(T) > len) {
			good = false;
		}
		if (good) {
			if constexpr (std::is_same_v<T, std::string>) {
				out = std::string((char*)(data + readPos));
				readPos += out.size() + 1;
			} else {
				memcpy(&out, data + readPos, sizeof(T));
				readPos += sizeof(T);
			}
		}
		return *this;
	}
};

#endif
