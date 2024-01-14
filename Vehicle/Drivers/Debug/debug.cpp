#include "debug.h"

#include <cstdio>
#include <memory>

#include "Drivers/interface.h"
#include "Stream/rc.h"

std::shared_ptr<Driver> createDriver(int argc, char** argv) {
	return std::make_shared<DebugDriver>(argc, argv);
}

void DebugDriver::update(const RCState& state) {
	printf("Acc. %.01f Brk. %.01f Str. %.03f\r", state.acceleration,
	       state.brakes, state.steering);
}
