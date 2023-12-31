#ifndef CAMERA_H
#define CAMERA_H

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "interface.h"
#include "libcamera/camera.h"
#include "libcamera/stream.h"

#define DST_BUFLEN 100
// "RC Video Stream" on a 10-key layout
#define DEFAULT_STREAM_PORT 7287

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
		bool raw        = false;
		bool stills     = false;
		bool video      = true;
		bool viewfinder = false;
	};

	enum StreamDestination : int {
		STREAM_IS_NAMED = 0b10,
		STREAM_HAS_PORT = 0b01,

		STREAM_TO_CONTROLLER = 0b01,
		STREAM_TO_EXTERNAL   = 0b11,
		STREAM_TO_DISK       = 0b10,
	};

private:
	struct RoleSelect selectedRoles;
	enum StreamDestination chosenDst = STREAM_TO_CONTROLLER;
	char streamDst[DST_BUFLEN]       = {0};
	int streamPort                   = DEFAULT_STREAM_PORT;

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

	enum StreamDestination& getStreamDst() { return chosenDst; }

	char* getStreamBuf() { return streamDst; }

	int& getStreamPort() { return streamPort; }

	size_t serialize(byte*) const;

	void deserialize(const byte*, size_t);
};

#endif
