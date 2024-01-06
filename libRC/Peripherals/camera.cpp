#include "camera.h"

#include <cstddef>
#include <cstdio>
#include <cstring>
#include <memory>
#include <vector>

#include "Stream/buffer.h"
#include "libcamera/camera.h"
#include "libcamera/stream.h"

const char* CameraState::decodeResult(const enum CameraResult result) {
	switch (result) {
		case CAMERA_OK:
			return "Camera operation successful";
		case BAD_CAMERA:
			return "Invalid camera";
		case START_FAILED:
			return "Failed to start camera";
		case ACQUIRE_FAILED:
			return "Failed to acquire camera";
		case BAD_CONFIG:
			return "Invalid camera configuration";
		case CONFIG_CHANGED:
			return "Camera configuration was changed";
		case BUFFER_ALLOC_FAILED:
			return "Failed to allocate frame buffers";
		case CREATE_REQUEST_FAILED:
			return "Failed to create frame request for camera";
		case QUEUE_REQUEST_FAILED:
			return "Failed to queue camera frame request";
		case SET_BUFFER_FAILED:
			return "Failed to set buffer for camera request";
		case STOP_FAILED:
			return "Failed to stop camera";
		case RELEASE_FAILED:
			return "Failed to release camera";
		default:
		case UNKNOWN_ERROR:
			return "Unknown error";
	}
}

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

bool CameraState::selectCamera(size_t idx) {
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
	enabled = true;
	return ret;
}

void CameraState::serialize(Buffer<byte>& buf) const {
	size_t written = 0;

	Metadata meta(*this);
	buf << meta;

	for (const auto& cam : cameras) {
		buf << cam;
	}
}

void CameraState::deserialize(Buffer<const byte>& buf) {
	size_t start = buf.tell();

	Metadata meta;
	buf >> meta;

	meta.retrieve(*this);
	for (int i = 0; i < meta.camCount; i++) {
		std::string cam;
		buf >> cam;
		cameras.emplace_back(std::move(cam));
	}

	deserializedSize = buf.tell() - start;
}
