#ifndef CAMERA_H
#define CAMERA_H

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "interface.h"
#include "libcamera/camera.h"

struct CameraState {
	struct __attribute__((packed)) Metadata {
		byte enabled;
		unsigned selectedCam;
		size_t camCount;
	};

	bool enabled = false;

	unsigned selectedCam = 0;
	std::vector<std::string> cameras;

	size_t deserializedSize = 0;

	void prepareCameraList(size_t);

	void loadCameraNames(
		const std::vector<std::shared_ptr<libcamera::Camera>>&);

	size_t serialize(byte*) const;

	void deserialize(const byte*, size_t);
};

#endif
