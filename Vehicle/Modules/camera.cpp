#include "camera.h"

#include <cstddef>
#include <cstring>
#include <memory>

#include "Peripherals/camera.h"
#include "Util/logging.h"
#include "interface.h"
#include "libcamera/camera_manager.h"
#include "modules.h"

Camera::Camera() {
	camMgr = std::make_unique<libcamera::CameraManager>();
	camMgr->start();
	queryState();
}

Camera::~Camera() {
	camMgr->stop();
}

void Camera::queryState() {
	camState.loadCameraNames(camMgr->cameras());
}

enum CameraState::CameraResult Camera::activateCamera() {
	if (camState.getSelected() >= camMgr->cameras().size()) {
		Logger::log(ERROR, "Tried to get camera at invalid index %d\n",
		            camState.getSelected());
		return CameraState::BAD_CAMERA;
	}
	camera = camMgr->cameras()[camState.getSelected()];
	if (camera->acquire() != 0) {
		return CameraState::ACQUIRE_FAILED;
	}
	Logger::log(DEBUG, "Acquired camera\n");
	return configureCamera();
}

enum CameraState::CameraResult Camera::configureCamera() {
	auto res = camState.configureCamera(camera);
	if (res == CameraState::BAD_CONFIG) {
		return res;
	}
	// TODO: allocate frame buffers and set up video output
	return res;
}

enum CameraState::CameraResult Camera::deactivateCamera(int& stop,
                                                        int& release) {
	if (camera) {
		stop = camera->stop();
		if (stop != 0) {
			return CameraState::STOP_FAILED;
		}
		release = camera->release();
		if (release != 0) {
			return CameraState::RELEASE_FAILED;
		}
		return CameraState::CAMERA_OK;
	}
	return CameraState::BAD_CAMERA;
}

void Camera::writeResult(const CameraResult res, struct Response& response) {
	if (res == CameraState::CAMERA_OK) {
		response << CAM_OK;
	} else {
		response << CAM_ERROR << res;
	}
}

void Camera::handleCameraDeactivation(struct Response& response) {
	int stop = 0, release = 0;
	auto res = deactivateCamera(stop, release);
	if (res == CameraState::CAMERA_OK) {
		response << CAM_OK;
	} else {
		if (stop == -ENODEV) {
			Logger::log(ERROR,
			            "Failed to stop camera: device disconnected (%d)\n",
			            stop);
		} else if (stop == -EACCES) {
			Logger::log(ERROR,
			            "Failed to stop camera: camera cannot be "
			            "stopped (%d)\n",
			            stop);
		}
		if (stop == -EBUSY) {
			Logger::log(ERROR,
			            "Failed to release camera: device is running (%d)\n",
			            release);
		}
		response << CAM_ERROR << res;
	}
}

// NOLINTNEXTLINE(misc-unused-parameters)
void Camera::respond(const byte* const msg, const size_t len,
                     struct Response& response) {
	switch (msg[0]) {
		case CAM_QUERY:
			response << CAM_STATE << camState;
			break;
		case CAM_DEACTIVATE:
			handleCameraDeactivation(response);
			break;
		case CAM_ACTIVATE:
			writeResult(activateCamera(), response);
			break;
		case CAM_CONFIGURE:
			writeResult(configureCamera(), response);
			break;
		default:
			Logger::log(
				ERROR,
				"Command 0x%02X was improperly dispatched to camera module\n",
				msg[0]);
			break;
	}
}
