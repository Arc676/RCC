#ifndef VEHICLE_MODULES_H
#define VEHICLE_MODULES_H

#include <concepts>
#include <cstddef>
#include <cstring>
#include <string>
#include <type_traits>

#include "Stream/buffer.h"
#include "Stream/netstream.h"
#include "interface.h"

template <typename T>
concept Serializable = requires(T x) {
	{ x.serialize((byte*)0) } -> std::convertible_to<size_t>;
};

/**
 * @brief A response to an incoming command
 */
class Response {
	byte data[MESSAGE_BUFLEN];
	size_t len;

	void append(const void* src, size_t size) {
		memcpy(data + len, src, size);
		len += size;
	}

public:
	size_t getSize() const { return len; }

	const byte* getBuffer() const { return data; }

	template <typename T>
	Response& operator<<(const T& in) {
		if constexpr (std::is_same_v<T, std::string>) {
			append(in.c_str(), in.length() + 1);
		} else {
			append(&in, sizeof(T));
		}
		return *this;
	}

	template <typename T>
		requires Serializable<T>
	Response& operator<<(const T& in) {
		len += in.serialize(data + len);
		return *this;
	}
};

class Responder {
protected:
	using Buf = Buffer<const byte>;

public:
	/**
	 * @brief Respond to an incoming command
	 *
	 * @param msg Command buffer
	 * @param response Response object in which to construct reply message, if
	 * any
	 */
	virtual void respond(Buf& msg, struct Response& response) = 0;

	virtual const char* name() const = 0;
};

#endif
