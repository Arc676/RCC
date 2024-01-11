#include "input.h"

#include <SDL_gamecontroller.h>
#include <SDL_keyboard.h>
#include <SDL_scancode.h>

#include <variant>

#include "Stream/rc.h"

InputSetupMap getDefaultInputs(RCState& state) {
	using RCS = RCState;
	using CH  = RCS::ControlHandler;

	// controller-agnostic inputs
	const CH accel{"Accelerate", state, &RCS::acceleration, RCS::CC_MAX};
	const CH brake{"Brake", state, &RCS::brakes, RCS::CC_MAX};

	InputSetupMap map{
		{accel, {SDL_SCANCODE_W, SDL_CONTROLLER_AXIS_TRIGGERRIGHT}},
		{brake, {SDL_SCANCODE_S, SDL_CONTROLLER_AXIS_TRIGGERLEFT}},
		{CH("Left", state, &RCS::steering, RCS::CC_MIN, true),
	     {SDL_SCANCODE_A, SDL_CONTROLLER_AXIS_INVALID}},
		{CH("Right", state, &RCS::steering, RCS::CC_MAX, true),
	     {SDL_SCANCODE_D, SDL_CONTROLLER_AXIS_INVALID}},
		{CH("Steering", state, &RCS::steering),
	     {SDL_SCANCODE_UNKNOWN, SDL_CONTROLLER_AXIS_LEFTX}}};

	return map;
}

bool isValidMapping(const ControlID& cid) {
	if (std::holds_alternative<SDL_Scancode>(cid)) {
		return std::get<SDL_Scancode>(cid) != SDL_SCANCODE_UNKNOWN;
	}
	if (std::holds_alternative<SDL_GameControllerAxis>(cid)) {
		return std::get<SDL_GameControllerAxis>(cid)
		       != SDL_CONTROLLER_AXIS_INVALID;
	}
	if (std::holds_alternative<SDL_GameControllerButton>(cid)) {
		return std::get<SDL_GameControllerButton>(cid)
		       != SDL_CONTROLLER_BUTTON_INVALID;
	}
	return false;
}

const char* getInputName(const ControlID& cid) {
	if (std::holds_alternative<SDL_Scancode>(cid)) {
		return SDL_GetScancodeName(std::get<SDL_Scancode>(cid));
	}
	if (std::holds_alternative<SDL_GameControllerAxis>(cid)) {
		return SDL_GameControllerGetStringForAxis(
			std::get<SDL_GameControllerAxis>(cid));
	}
	if (std::holds_alternative<SDL_GameControllerButton>(cid)) {
		return SDL_GameControllerGetStringForButton(
			std::get<SDL_GameControllerButton>(cid));
	}
	return nullptr;
}

InputMap createInputMap(const InputSetupMap& ism) {
	InputMap map;
	for (const auto& [ch, controls] : ism) {
		const auto& [kb, joystick] = controls;
		if (isValidMapping(kb)) {
			map.emplace(kb, ch);
		}
		if (isValidMapping(joystick)) {
			map.emplace(joystick, ch);
		}
	}
	return map;
}
