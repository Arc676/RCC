#include "buffer.h"

#include "interface.h"

template <typename Data>
	requires IsByte<Data>
void Buffer<Data>::append(const void* src, size_t size)
	requires(!std::is_const_v<Data>)
{
	if (len + size >= capacity) {
		good = false;
		return;
	}
	memcpy(data + len, src, size);
	len += size;
}

template <typename Data>
	requires IsByte<Data>
Buffer<Data>::Buffer(size_t capacity)
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
