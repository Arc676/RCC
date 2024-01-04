#include <SDL_scancode.h>

#include "Stream/rc.h"
#include "module.h"
#include "rc.h"

RCModule::RCModule(const Dashboard* dash)
	: Module(dash) {
	using RCS = RCState;
	using CH  = RCS::ControlHandler;

	controls[KEYBOARD] = {
		{SDL_SCANCODE_W, CH(state, &RCS::acceleration)},
		{SDL_SCANCODE_S, CH(state, &RCS::brakes)},
		{SDL_SCANCODE_A, CH(state, &RCS::steering, RCS::CC_MIN, true)},
		{SDL_SCANCODE_D, CH(state, &RCS::steering, RCS::CC_MAX, true)},
	};
	// TBD
	controls[JOYSTICK] = InputMap();
}
