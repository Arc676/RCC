#ifndef CAMERA_MODULE_H
#define CAMERA_MODULE_H

#include <cstddef>
#include <memory>

#include "Peripherals/camera.h"
#include "libcamera/camera_manager.h"
#include "libcamera/libcamera.h"
#include "libcamera/stream.h"
#include "modules.h"

class Camera : public Responder {
	CameraState camState;
	std::unique_ptr<libcamera::CameraManager> camMgr;

	void queryState();

public:
	Camera();

	~Camera();

	const char* name() const override { return "Camera"; }

	void respond(const byte*, size_t, struct Response&) override;
};

#endif
