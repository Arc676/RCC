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
	auto res = camState.configureCamera(camera);
	if (res != CameraState::CAMERA_OK) {
		return res;
	}
	// TODO: allocate frame buffers and set up video output
	return CameraState::CAMERA_OK;
}

void Camera::deactivateCamera() {
	if (camera) {
		camera->release();
	}
}

// NOLINTNEXTLINE(misc-unused-parameters)
void Camera::respond(const byte* const msg, const size_t len,
                     struct Response& response) {
	switch (msg[0]) {
		case CAM_QUERY: {
			response.data[0]     = CAM_STATE;
			const size_t written = camState.serialize(response.data + 1);
			response.len         = 1 + written;
			break;
		}
		case CAM_DEACTIVATE:
			deactivateCamera();
			response.setOneByte(CAM_OK);
			break;
		case CAM_ACTIVATE: {
			auto res = activateCamera();
			if (res == CameraState::CAMERA_OK) {
				response.setOneByte(CAM_OK);
			} else {
				response.data[0] = CAM_ERROR;
				memcpy(response.data + 1, &res,
				       sizeof(enum CameraState::CameraResult));
				response.len = 1 + sizeof(enum CameraState::CameraResult);
			}
			break;
		}
		default:
			Logger::log(
				ERROR,
				"Command 0x%02X was improperly dispatched to camera module\n",
				msg[0]);
			break;
	}
}
