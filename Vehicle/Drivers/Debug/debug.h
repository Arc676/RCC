#ifndef DEBUG_DRIVER_H
#define DEBUG_DRIVER_H

#include "Drivers/interface.h"
#include "Stream/rc.h"

class DebugDriver : public Driver {
public:
	DebugDriver(int /*unused*/, char** /*unused*/) {}

	void update(const RCState& state) override;
};

#endif
