#include "camera.h"

#include <cstddef>
#include <cstdio>
#include <cstring>
#include <memory>
#include <vector>

#include "libcamera/camera.h"

void CameraState::prepareCameraList(const size_t count) {
	cameras.reserve(count);
	cameras.clear();
}

void CameraState::loadCameraNames(
	const std::vector<std::shared_ptr<libcamera::Camera>>& cams) {
	prepareCameraList(cams.size());
	for (const auto& cam : cams) {
		cameras.push_back(cam->id());
	}
}

bool CameraState::selectCamera(unsigned idx) {
	if (idx >= cameras.size()) {
		return false;
	}
	selectedCam = idx;
	return true;
}

size_t CameraState::serialize(byte* buf) const {
	size_t written = 0;
	Metadata meta{(byte)enabled, selectedCam, cameras.size()};
	memcpy((void*)buf, &meta, sizeof(Metadata));
	written += sizeof(Metadata);
	for (const auto& cam : cameras) {
		size_t len = cam.length();
		memcpy(buf + written, cam.c_str(), len + 1);
		written += len + 1;
	}
	return written;
}

void CameraState::deserialize(const byte* buf, const size_t len) {
	Metadata meta;
	memcpy(&meta, (void*)buf, sizeof(Metadata));
	enabled     = (meta.enabled != 0U);
	selectedCam = meta.selectedCam;
	size_t pos  = sizeof(Metadata);
	prepareCameraList(meta.camCount);
	for (int i = 0; i < meta.camCount; i++) {
		cameras.emplace_back((char*)buf + pos);
		pos += cameras.back().size() + 1;
	}
	deserializedSize = len;
}
