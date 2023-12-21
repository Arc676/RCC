#include "camera.h"

#include <cstddef>
#include <memory>

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
		default:
			Logger::log(
				ERROR,
				"Command 0x%02X was improperly dispatched to camera module\n",
				msg[0]);
			break;
	}
}
