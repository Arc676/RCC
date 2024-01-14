#include "debug.h"

#include <cstdio>
#include <memory>

#include "Drivers/interface.h"
#include "Stream/rc.h"

DEFAULT_DRIVER_CREATOR(DebugDriver)

void DebugDriver::update(const RCState& state) {
	printf("Acc. %.01f Brk. %.01f Str. %.03f\r", state.acceleration,
	       state.brakes, state.steering);
}
