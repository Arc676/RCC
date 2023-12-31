#ifndef CAMERA_H
#define CAMERA_H

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "interface.h"
#include "libcamera/camera.h"

class CameraState {
	bool enabled = false;

	unsigned selectedCam = 0;
	std::vector<std::string> cameras;

	size_t deserializedSize = 0;

	void prepareCameraList(size_t);

public:
	struct __attribute__((packed)) Metadata {
		byte enabled;
		unsigned selectedCam;
		size_t camCount;
	};

	void loadCameraNames(
		const std::vector<std::shared_ptr<libcamera::Camera>>&);

	const std::vector<std::string>& getCameraNames() const { return cameras; }

	size_t getDeserializedSize() const { return deserializedSize; }

	bool cameraIsEnabled() const { return enabled; }

	unsigned getSelected() const { return selectedCam; }

	bool selectCamera(unsigned);

	size_t serialize(byte*) const;

	void deserialize(const byte*, size_t);
};

#endif
