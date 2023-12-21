#ifndef CAMERA_MODULE_H
#define CAMERA_MODULE_H

#include <cstddef>

#include "Peripherals/camera.h"
#include "interface.h"
#include "module.h"

class CameraModule : public Module {
	CameraState state;
	std::string viewfinderTitle;

protected:
	void renderController();

	void renderViewfinder();

public:
	void render() override;

	bool canHandleMessage(const byte opCode) const override {
		return (opCode & CAMERA_FLAG) != 0;
	}

	void handleMessage(const byte*, size_t) override;
};

#endif
