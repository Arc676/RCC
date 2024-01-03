#include "camdata.h"

#include <cstring>

size_t CameraData::serialize(byte* const buf) const {
	if (!exists()) {
		return 0;
	}

	size_t written = 0;

	const auto& props = camera->properties();

	size_t len = props.size();
	memcpy(buf + written, &len, sizeof(size_t));
	written += sizeof(size_t);

	for (const auto& [id, value] : props) {
		memcpy(buf + written, &id, sizeof(unsigned int));
		written += sizeof(unsigned int);

		auto data  = value.toString();
		size_t len = data.size() + 1;
		memcpy(buf + written, data.c_str(), len);
		written += len;
	}

	return written;
}

CameraData::CameraProperties
// NOLINTNEXTLINE(misc-unused-parameters)
CameraData::deserialize(const byte* const buf, const size_t len) {
	CameraData::CameraProperties props;

	size_t pos = 0;

	size_t count;
	memcpy(&count, buf + pos, sizeof(size_t));
	pos += sizeof(size_t);

	for (int i = 0; i < count; i++) {
		unsigned int key;
		memcpy(&key, buf + pos, sizeof(unsigned int));
		pos += sizeof(unsigned int);
		std::string data((char*)buf + pos);
		pos += data.size() + 1;
		props[key] = std::move(data);
	}

	return props;
}
