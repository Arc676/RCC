#include "camera.h"

#include <cstddef>
#include <cstring>
#include <memory>
#include <utility>

#include "Peripherals/camera.h"
#include "Stream/camdata.h"
#include "Util/logging.h"
#include "interface.h"
#include "libcamera/camera_manager.h"
#include "libcamera/framebuffer.h"
#include "libcamera/framebuffer_allocator.h"
#include "libcamera/request.h"
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

enum CameraState::CameraResult Camera::startCamera(int& start) {
	start = camera->start();
	camState.startCamera();
	if (start == 0) {
		for (auto& request : requests) {
			start = camera->queueRequest(request.get());
			if (start != 0) {
				return CameraState::QUEUE_REQUEST_FAILED;
			}
		}
		return CameraState::CAMERA_OK;
	}
	return CameraState::START_FAILED;
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
	// set up event handlers
	camera->requestCompleted.connect(this, &Camera::requestCompleted);
	camera->bufferCompleted.connect(this, &Camera::bufferCompleted);
	return res;
}

void Camera::requestCompleted(libcamera::Request* const request) {
	// skip cancelled requests
	if (request->status() == libcamera::Request::RequestCancelled) {
		return;
	}

	// TODO: process data

	// reuse request
	request->reuse(libcamera::Request::ReuseBuffers);
	camera->queueRequest(request);
}

void Camera::bufferCompleted(libcamera::Request* const request,
                             libcamera::FrameBuffer* const buf) {}

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
		allocator.reset();
		release = camera->release();
		if (release != 0) {
			return CameraState::RELEASE_FAILED;
		}
		camera.reset();
		camState.deactivateCamera();
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
		if (release == -EBUSY) {
			Logger::log(ERROR,
			            "Failed to release camera: device is running (%d)\n",
			            release);
		}
		response << CAM_ERROR << res;
	}
}

void Camera::respond(const byte* const msg, const size_t len,
                     struct Response& response) {
	switch (msg[0]) {
		case CAM_QUERY:
			if (len >= 1 + sizeof(size_t)) {
				size_t idx;
				memcpy(&idx, msg + 1, sizeof(size_t));
				if (idx < camMgr->cameras().size()) {
					auto cam = CameraData(camMgr->cameras()[idx]);
					response << CAM_PROPS << cam;
				} else {
					response << CAM_ERROR << CameraState::BAD_CAMERA;
				}
			} else {
				response << CAM_STATE << camState;
			}
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
		case CAM_START: {
			int start;
			writeResult(startCamera(start), response);
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
