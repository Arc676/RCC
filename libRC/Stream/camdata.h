#ifndef CAMERA_DATA_H
#define CAMERA_DATA_H

#include <memory>

#include "interface.h"
#include "libcamera/camera.h"

class CameraData {
	using SharedCamera = std::shared_ptr<libcamera::Camera>;
	SharedCamera camera;

public:
	CameraData(const SharedCamera& camera)
		: camera(camera) {}

	using CameraProperties = std::unordered_map<unsigned int, std::string>;

	bool exists() const { return camera != nullptr; }

	size_t serialize(byte*) const;

	static CameraProperties deserialize(const byte*, size_t);
};

#endif
