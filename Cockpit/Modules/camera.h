#ifndef CAMERA_MODULE_H
#define CAMERA_MODULE_H

#include <cstddef>
#include <string>

#include "Peripherals/camera.h"
#include "Stream/camdata.h"
#include "interface.h"
#include "module.h"

class CameraModule : public Module {
	CameraState state;
	std::string viewfinderTitle;
	enum CameraState::CameraResult lastResult = CameraState::CAMERA_OK;
	CameraData::CameraProperties camProps;

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
