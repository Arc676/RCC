#include "camera.h"

#include <cstddef>
#include <cstring>
#include <memory>
#include <utility>

#include "Peripherals/camera.h"
#include "Util/logging.h"
#include "interface.h"
#include "libcamera/camera_manager.h"
#include "libcamera/framebuffer_allocator.h"
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
	// configure camera
	auto res = camState.configureCamera(camera);
	if (res == CameraState::BAD_CONFIG) {
		return res;
	}
	// allocate frame buffers
	const auto& cfg = *camState.getConfiguration();
	allocator       = std::make_unique<libcamera::FrameBufferAllocator>(camera);
	for (const auto& streamCfg : cfg) {
		if (allocator->allocate(streamCfg.stream()) < 0) {
			return CameraState::BUFFER_ALLOC_FAILED;
		}
		Logger::log(DEBUG, "Allocated %d buffers for stream\n",
		            allocator->buffers(streamCfg.stream()).size());
	}
	// set up frame capture requests
	for (const auto& streamCfg : cfg) {
		const auto& buffers = allocator->buffers(streamCfg.stream());
		for (const auto& buffer : buffers) {
			auto request = camera->createRequest();
			if (!request) {
				return CameraState::CREATE_REQUEST_FAILED;
			}
			if (request->addBuffer(streamCfg.stream(), buffer.get()) < 0) {
				return CameraState::SET_BUFFER_FAILED;
			}
			requests.push_back(std::move(request));
		}
	}
	// TODO: set up event handlers
	return res;
}

enum CameraState::CameraResult Camera::deactivateCamera(int& stop,
                                                        int& release) {
	if (camera) {
		stop = camera->stop();
		if (stop != 0) {
			return CameraState::STOP_FAILED;
		}
		for (const auto& streamCfg : *camState.getConfiguration()) {
			allocator->free(streamCfg.stream());
		}
		release = camera->release();
		if (release != 0) {
			return CameraState::RELEASE_FAILED;
		}
		camera.reset();
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
