#ifndef CAMERA_H
#define CAMERA_H

#include <cstddef>
#include <memory>
#include <string>
#include <vector>

#include "Stream/buffer.h"
#include "interface.h"
#include "libcamera/camera.h"
#include "libcamera/stream.h"

#define DST_BUFLEN 100
// "RC Video Stream" on a 10-key layout
#define DEFAULT_STREAM_PORT 7287

class CameraState {
	bool enabled = false;
	bool running = false;

	size_t selectedCam = 0;
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

	struct __attribute__((packed)) Metadata {
		// camera state
		bool enabled : 1;
		bool running : 1;
		// camera roles
		bool rawEnabled : 1;
		bool stillsEnabled : 1;
		bool videoEnabled : 1;
		bool viewfinderEnabled : 1;
		// stream output
		enum StreamDestination dst : 2;

		// user config
		size_t selectedCam;
		size_t camCount;

		Metadata() {}

		Metadata(const CameraState& cam) { store(cam); }

		void store(const CameraState& cam) {
			enabled     = cam.enabled;
			running     = cam.running;
			selectedCam = cam.selectedCam;
			camCount    = cam.cameras.size();

			rawEnabled        = cam.selectedRoles.raw;
			stillsEnabled     = cam.selectedRoles.stills;
			videoEnabled      = cam.selectedRoles.video;
			viewfinderEnabled = cam.selectedRoles.viewfinder;

			dst = cam.chosenDst;
		}

		void retrieve(CameraState& cam) const {
			cam.enabled     = enabled;
			cam.running     = running;
			cam.selectedCam = selectedCam;
			cam.prepareCameraList(camCount);

			cam.selectedRoles.raw        = rawEnabled;
			cam.selectedRoles.stills     = stillsEnabled;
			cam.selectedRoles.video      = videoEnabled;
			cam.selectedRoles.viewfinder = viewfinderEnabled;

			cam.chosenDst = dst;
		}
	};

public:
	enum CameraResult {
		CAMERA_OK,
		BAD_CAMERA,
		START_FAILED,
		ACQUIRE_FAILED,
		BAD_CONFIG,
		CONFIG_CHANGED,
		BUFFER_ALLOC_FAILED,
		CREATE_REQUEST_FAILED,
		QUEUE_REQUEST_FAILED,
		SET_BUFFER_FAILED,
		STOP_FAILED,
		RELEASE_FAILED,
		UNKNOWN_ERROR,
	};

	static const char* decodeResult(enum CameraResult);

	void loadCameraNames(const std::vector<SharedCamera>&);

	const std::vector<std::string>& getCameraNames() const { return cameras; }

	size_t getDeserializedSize() const { return deserializedSize; }

	bool cameraIsEnabled() const { return enabled; }

	bool cameraIsRunning() const { return running; }

	size_t getSelected() const { return selectedCam; }

	bool selectCamera(size_t);

	enum CameraResult configureCamera(const SharedCamera&);

	void startCamera() { running = true; }

	void deactivateCamera() {
		enabled = false;
		running = false;
	}

	const libcamera::CameraConfiguration* getConfiguration() const {
		return config.get();
	}

	RoleSelect& getRoles() { return selectedRoles; }

	enum StreamDestination& getStreamDst() { return chosenDst; }

	char* getStreamBuf() { return streamDst; }

	int& getStreamPort() { return streamPort; }

	void serialize(Buffer<byte>&) const;

	void deserialize(Buffer<const byte>&);
};

#endif
