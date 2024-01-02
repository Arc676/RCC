#ifndef CAMERA_MODULE_H
#define CAMERA_MODULE_H

#include <cstddef>
#include <memory>
#include <vector>

#include "Peripherals/camera.h"
#include "libcamera/camera_manager.h"
#include "libcamera/framebuffer_allocator.h"
#include "libcamera/libcamera.h"
#include "libcamera/request.h"
#include "libcamera/stream.h"
#include "modules.h"

class Camera : public Responder {
	using SharedCamera = CameraState::SharedCamera;
	using CameraResult = enum CameraState::CameraResult;

	CameraState camState;
	std::unique_ptr<libcamera::CameraManager> camMgr;
	SharedCamera camera = nullptr;
	std::unique_ptr<libcamera::FrameBufferAllocator> allocator;
	std::vector<std::unique_ptr<libcamera::Request>> requests;

	void queryState();

	CameraResult activateCamera();

	CameraResult configureCamera();

	CameraResult deactivateCamera(int&, int&);

	void handleCameraDeactivation(struct Response&);

	static void writeResult(CameraResult, struct Response&);

public:
	Camera();

	~Camera();

	const char* name() const override { return "Camera"; }

	void respond(const byte*, size_t, struct Response&) override;
};

#endif
