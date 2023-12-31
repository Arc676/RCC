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
		return CameraState::BAD_CAMERA;
	}
	camera = camMgr->cameras()[camState.getSelected()];
	if (camera->acquire() != 0) {
		return CameraState::ACQUIRE_FAILED;
	}
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

void Camera::deactivateCamera() {
	if (camera) {
		camera->stop();
		camera->release();
	}
}

void Camera::writeResult(const enum CameraState::CameraResult res,
                         struct Response& response) {
	if (res == CameraState::CAMERA_OK) {
		response << CAM_OK;
	} else {
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
			deactivateCamera();
			response << CAM_OK;
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
