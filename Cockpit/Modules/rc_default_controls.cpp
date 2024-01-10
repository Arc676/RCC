#include <SDL_scancode.h>

#include "Stream/rc.h"
#include "module.h"
#include "rc.h"

RCModule::RCModule(const Dashboard* dash)
	: Module(dash) {
	using RCS = RCState;
	using CH  = RCS::ControlHandler;

	// controls for either input type
	const CH accel{"Accelerate", state, &RCS::acceleration};
	const CH brake{"Brake", state, &RCS::brakes};

	controls[KEYBOARD] = {
		{SDL_SCANCODE_W, accel},
		{SDL_SCANCODE_S, brake},
		{SDL_SCANCODE_A, CH("Left", state, &RCS::steering, RCS::CC_MIN, true)},
		{SDL_SCANCODE_D, CH("Right", state, &RCS::steering, RCS::CC_MAX, true)},
	};
	// TBD
	controls[JOYSTICK] = InputMap();
}
