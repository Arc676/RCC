#include <cstdio>

#include "Stream/rc.h"

void update(const RCState& state) {
	printf("Acc. %.01f Brk. %.01f Str. %.03f\r", state.acceleration,
	       state.brakes, state.steering);
}
