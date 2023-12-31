#ifndef CAMERA_H
#define CAMERA_H

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "interface.h"
#include "libcamera/camera.h"
#include "libcamera/stream.h"

class CameraState {
	bool enabled = false;

	unsigned selectedCam = 0;
	std::vector<std::string> cameras;

	std::unique_ptr<libcamera::CameraConfiguration> config = nullptr;

	size_t deserializedSize = 0;

	void prepareCameraList(size_t);

public:
	using SharedCamera = std::shared_ptr<libcamera::Camera>;

	struct RoleSelect {
		bool raw;
		bool stills;
		bool video = true;
		bool viewfinder;
	};

private:
	RoleSelect selectedRoles;

	std::vector<libcamera::StreamRole> getRoleVec() const;

public:
	struct __attribute__((packed)) Metadata {
		byte enabled;
		unsigned selectedCam;
		size_t camCount;
	};

	enum CameraResult {
		CAMERA_OK,
		BAD_CAMERA,
		ACQUIRE_FAILED,
		BAD_CONFIG,
		CONFIG_CHANGED,
		BUFFER_ALLOC_FAILED,
		STOP_FAILED,
		RELEASE_FAILED,
	};

	void loadCameraNames(const std::vector<SharedCamera>&);

	const std::vector<std::string>& getCameraNames() const { return cameras; }

	size_t getDeserializedSize() const { return deserializedSize; }

	bool cameraIsEnabled() const { return enabled; }

	unsigned getSelected() const { return selectedCam; }

	bool selectCamera(unsigned);

	enum CameraResult configureCamera(const SharedCamera&);

	RoleSelect& getRoles() { return selectedRoles; }

	size_t serialize(byte*) const;

	void deserialize(const byte*, size_t);
};

#endif
