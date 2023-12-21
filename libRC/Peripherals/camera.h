#ifndef CAMERA_H
#define CAMERA_H

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "interface.h"
#include "libcamera/camera.h"
struct CameraState {
	bool enabled = false;

	unsigned selectedCam = 0;
	std::vector<std::string> cameras;

	void loadCameraNames(
		const std::vector<std::shared_ptr<libcamera::Camera>>&);

	size_t serialize(byte*) const;

	void deserialize(const byte*, size_t);
};

#endif
