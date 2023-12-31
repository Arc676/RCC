#include "camera.h"

#include <cstddef>
#include <cstdio>
#include <cstring>
#include <memory>
#include <vector>

#include "libcamera/camera.h"
#include "libcamera/stream.h"

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

std::vector<libcamera::StreamRole> CameraState::getRoleVec() const {
	std::vector<libcamera::StreamRole> roles;
	if (selectedRoles.raw) {
		roles.push_back(libcamera::StreamRole::Raw);
	}
	if (selectedRoles.stills) {
		roles.push_back(libcamera::StreamRole::StillCapture);
	}
	if (selectedRoles.video) {
		roles.push_back(libcamera::StreamRole::VideoRecording);
	}
	if (selectedRoles.viewfinder) {
		roles.push_back(libcamera::StreamRole::Viewfinder);
	}
	return roles;
}

enum CameraState::CameraResult CameraState::configureCamera(
	const SharedCamera& cam) {
	config   = cam->generateConfiguration(getRoleVec());
	auto res = config->validate();
	auto ret = CAMERA_OK;
	if (res == libcamera::CameraConfiguration::Invalid) {
		return BAD_CONFIG;
	}
	if (res == libcamera::CameraConfiguration::Adjusted) {
		ret = CONFIG_CHANGED;
	}
	cam->configure(config.get());
	return ret;
}

size_t CameraState::serialize(byte* buf) const {
	size_t written = 0;

	struct Metadata meta {
		(byte) enabled, selectedCam, cameras.size()
	};
	memcpy((void*)buf, &meta, sizeof(struct Metadata));
	written += sizeof(struct Metadata);

	for (const auto& cam : cameras) {
		size_t len = cam.length();
		memcpy(buf + written, cam.c_str(), len + 1);
		written += len + 1;
	}

	// Compactly send boolean values
	// Bits 1-4: selected roles
	// Bits 5, 6: stream destination
	// NOLINTBEGIN(readability-magic-numbers)
	byte b = 0;
	if (selectedRoles.raw) {
		b |= 1 << 0;
	}
	if (selectedRoles.stills) {
		b |= 1 << 1;
	}
	if (selectedRoles.video) {
		b |= 1 << 2;
	}
	if (selectedRoles.viewfinder) {
		b |= 1 << 3;
	}
	b |= chosenDst << 4;
	buf[written++] = b;
	// NOLINTEND(readability-magic-numbers)

	if ((chosenDst & STREAM_IS_NAMED) != 0) {
		size_t len = strlen(streamDst) + 1;
		memcpy(buf + written, streamDst, len);
		written += len;
	}
	if ((chosenDst & STREAM_HAS_PORT) != 0) {
		memcpy(buf + written, &streamPort, sizeof(int));
		written += sizeof(int);
	}

	return written;
}

void CameraState::deserialize(const byte* buf, const size_t len) {
	struct Metadata meta;
	memcpy(&meta, (void*)buf, sizeof(struct Metadata));
	enabled     = (meta.enabled != 0U);
	selectedCam = meta.selectedCam;
	size_t pos  = sizeof(struct Metadata);

	prepareCameraList(meta.camCount);
	for (int i = 0; i < meta.camCount; i++) {
		cameras.emplace_back((char*)buf + pos);
		pos += cameras.back().size() + 1;
	}

	// See CameraState::serialize for bits
	byte b                   = buf[pos++];
	selectedRoles.raw        = (b & (1 << 0)) != 0;
	selectedRoles.stills     = (b & (1 << 1)) != 0;
	selectedRoles.video      = (b & (1 << 2)) != 0;
	selectedRoles.viewfinder = (b & (1 << 3)) != 0;
	chosenDst                = (enum StreamDestination)((b >> 4) & 0b11);

	if ((chosenDst & STREAM_IS_NAMED) != 0) {
		size_t len = strlen(streamDst) + 1;
		memcpy(streamDst, buf + pos, len);
		pos += len;
	}
	if ((chosenDst & STREAM_HAS_PORT) != 0) {
		memcpy(&streamPort, buf + pos, sizeof(int));
		pos += sizeof(int);
	}

	deserializedSize = len;
}
