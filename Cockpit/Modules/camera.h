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
	void cameraSelect();

	void roleSelect();

	void dstSelect();

	void cameraControls();

	void renderController();

	void renderViewfinder();

public:
	void render() override;

	bool canHandleMessage(const byte opCode) const override {
		return (opCode & OPCODE_ID_BITMASK) == CAMERA_CMD;
	}

	void handleMessage(const byte*, size_t) override;
};

#endif
