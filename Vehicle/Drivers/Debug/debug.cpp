#include <cstdio>

#include "Drivers/interface.h"
#include "Stream/rc.h"

void Driver::update(const RCState& state) {
	printf("Acc. %.01f Brk. %.01f Str. %.03f\r", state.acceleration,
	       state.brakes, state.steering);
}
