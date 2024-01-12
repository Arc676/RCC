#include "buffer.h"

#include <algorithm>
#include <type_traits>

#include "interface.h"

template <typename Data>
	requires IsByte<Data>
Buffer<Data>::Buffer(const Buffer& other)
	requires Writable<Data>
	: Buffer(other.capacity) {
	const auto* src = other.data;
	// NOLINTNEXTLINE(*pointer-arithmetic)
	std::copy(src, src + capacity, data);
}

template <typename Data>
	requires IsByte<Data>
Buffer<Data>::Buffer(const Buffer& other)
	requires(!Writable<Data>)
	: Buffer(other.data, other.capacity) {}

template <typename Data>
	requires IsByte<Data>
Buffer<Data>::Buffer(Buffer&& other) noexcept
	: Buffer(other.data, other.capacity) {
	// NOLINTNEXTLINE(*member-initializer)
	owned       = other.owned;
	other.owned = false;
}

template <typename Data>
	requires IsByte<Data>
Buffer<Data>& Buffer<Data>::operator=(const Buffer& other) {
	if (this != &other) {
		Buffer<Data> copy{other};
		*this = std::move(copy);
	}
	return *this;
}

template <typename Data>
	requires IsByte<Data>
Buffer<Data>& Buffer<Data>::operator=(Buffer&& other) noexcept {
	data        = other.data;
	owned       = other.owned;
	other.owned = false;
	return *this;
}

template <typename Data>
	requires IsByte<Data>
void Buffer<Data>::append(const void* src, size_t size)
	requires Writable<Data>
{
	if (len + size >= capacity) {
		good = false;
		return;
	}
	// NOLINTNEXTLINE(*pointer-arithmetic)
	memcpy(data + len, src, size);
	len += size;
}

template <typename Data>
	requires IsByte<Data>
Buffer<Data>::Buffer(size_t capacity)
	requires Writable<Data>
	: data(new byte[capacity]())
	, capacity(capacity) {}

template <typename Data>
	requires IsByte<Data>
Buffer<Data>::Buffer(Data* buf, size_t size)
	: data(buf)
	, len(size)
	, capacity(size)
	, owned(false) {}

template <typename Data>
	requires IsByte<Data>
Buffer<Data>::~Buffer() {
	if (owned) {
		delete[] data;
	}
}

template class Buffer<byte>;
template class Buffer<const byte>;
