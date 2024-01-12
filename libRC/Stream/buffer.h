#ifndef BUFFER_H
#define BUFFER_H

#include <cstddef>
#include <cstring>
#include <string>
#include <type_traits>

#include "Stream/netstream.h"
#include "interface.h"

/**
 * @brief Identifies one-byte data types
 *
 * @tparam T Data type
 */
template <typename T>
concept IsByte = sizeof(T) == 1;

/**
 * @brief Buffer class for managing raw data
 *
 * @tparam T Underlying data type for the buffer
 */
template <typename T>
	requires IsByte<T>
class Buffer;

/**
 * @brief Identifies serializable data types
 *
 * @tparam T The data type to serialize
 * @tparam U The buffer data type into which to serialize
 */
template <typename T, typename U>
concept Serializable = requires(const T x) {
	{ x.serialize(*(Buffer<U>*)0) };
};

/**
 * @brief Identifies deserializable data types
 *
 * @tparam T The data type to deserialize
 * @tparam U The buffer data type from which to deserialize
 */
template <typename T, typename U>
concept Deserializable = requires(T x) {
	{ x.deserialize(*(Buffer<const U>*)0) };
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

	/**
	 * @brief Appends data to the buffer, if there is enough space (requires
	 * that the buffer be writable)
	 *
	 * @param src Pointer to data
	 * @param size Size of data
	 */
	void append(const void* src, size_t size)
		requires(!std::is_const_v<Data>);

public:
	/**
	 * @brief Construct a new Buffer object with new heap space
	 *
	 * @param capacity Buffer capacity
	 */
	explicit Buffer(size_t capacity = MESSAGE_BUFLEN);

	/**
	 * @brief Construct a new Buffer object with existing heap space (assumes
	 * that the buffer is already completely filled with data)
	 *
	 * @param buf Buffer
	 * @param size Buffer capacity
	 */
	Buffer(Data* buf, size_t size);

	~Buffer();

	/**
	 * @brief Get buffer size
	 *
	 * @return Number of bytes of data present in the buffer
	 */
	[[nodiscard]] size_t size() const { return len; }

	/**
	 * @brief Get the buffer
	 *
	 * @return Read-only pointer to the buffer data
	 */
	const Data* getBuffer() const { return data; }

	/**
	 * @brief Peeks at the next byte in the buffer
	 *
	 * @return Byte currently under the read position
	 */
	Data peek() const { return data[readPos]; }

	/**
	 * @brief Query buffer state
	 *
	 * @return Whether the last I/O operation succeeded
	 */
	[[nodiscard]] bool ok() const { return good; }

	/**
	 * @brief Reset buffer state
	 */
	void reset() { good = true; }

	/**
	 * @brief Mark the buffer as empty
	 */
	void clear() {
		len = 0;
		reset();
	}

	/**
	 * @brief Query buffer read position
	 *
	 * @return Current offset from which read operations will occur
	 */
	[[nodiscard]] size_t tell() const { return readPos; }

	/**
	 * @brief Reset read position to beginning of buffer
	 */
	void rewind() { readPos = 0; }

	/**
	 * @brief Write object to buffer (requires that the buffer be writable)
	 *
	 * @tparam T Data type to write
	 * @param in Object to write
	 * @return Reference to the buffer object (like with streams)
	 */
	template <typename T>
	Buffer& operator<<(const T& in)
		requires(!std::is_const_v<Data>)
	{
		if (good) {
			if constexpr (Serializable<T, Data>) {
				in.serialize(*this);
			} else if constexpr (std::is_same_v<T, std::string>) {
				append(in.c_str(), in.length() + 1);
			} else {
				append(&in, sizeof(T));
			}
		}
		return *this;
	}

	/**
	 * @brief Read object from buffer
	 *
	 * @tparam T Data type to read
	 * @param out Object into which to write data
	 * @return Reference to the buffer object (like with streams)
	 */
	template <typename T>
	Buffer& operator>>(T& out) {
		if constexpr (Deserializable<T, Data>) {
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
