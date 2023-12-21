#include "camera.h"

#include <cstddef>
#include <memory>
#include <vector>

#include "libcamera/camera.h"

void CameraState::loadCameraNames(
	const std::vector<std::shared_ptr<libcamera::Camera>>& cams) {
	cameras.reserve(cams.size());
	cameras.clear();
	for (const auto& cam : cams) {
		cameras.push_back(cam->id());
	}
}

size_t CameraState::serialize(byte* buf) const {}

void CameraState::deserialize(const byte* buf, const size_t len) {}
