#include "buffer.h"

#include "interface.h"

void Buffer::append(const void* src, size_t size) {
	if (len + size >= capacity) {
		good = false;
		return;
	}
	memcpy(data + len, src, size);
	len += size;
}

Buffer::Buffer(size_t capacity)
	: capacity(capacity) {
	data = new byte[capacity]();
}

Buffer::Buffer(byte* buf, size_t capacity)
	: data(buf)
	, capacity(capacity)
	, owned(false) {}

Buffer::~Buffer() {
	if (owned) {
		delete[] data;
	}
}
