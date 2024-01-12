#include "camdata.h"

#include <cstring>
#include <string>

#include "Stream/buffer.h"

void CameraData::serialize(Buffer<byte>& buf) const {
	if (!exists()) {
		return;
	}

	const auto& props = camera->properties();
	buf << props.size();
	for (const auto& [id, value] : props) {
		buf << id << value.toString();
	}
}

void CameraProperties::deserialize(Buffer<const byte>& buf) {
	props.clear();

	size_t count = 0;
	buf >> count;

	for (int i = 0; i < count; i++) {
		unsigned int key = 0;
		std::string data;
		buf >> key >> data;
		props[key] = std::move(data);
	}
}

CameraProperties::iterator CameraProperties::begin() {
	return props.begin();
}

CameraProperties::iterator CameraProperties::end() {
	return props.end();
}

CameraProperties::const_iterator CameraProperties::cbegin() const {
	return props.begin();
}

CameraProperties::const_iterator CameraProperties::cend() const {
	return props.end();
}
