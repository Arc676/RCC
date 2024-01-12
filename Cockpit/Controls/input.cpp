#include "input.h"

#include <SDL_gamecontroller.h>
#include <SDL_keyboard.h>
#include <SDL_scancode.h>

#include <cstddef>
#include <cstdio>
#include <stdexcept>
#include <utility>
#include <variant>

#include "Stream/rc.h"

InputSetupMap getDefaultInputs(RCState& state) {
	using RCS = RCState;
	using CH  = RCS::ControlHandler;

	InputSetupMap map{
		// keyboard-only inputs
		{CH(CH::KB_ACCEL, state, &RCS::acceleration, RCS::CC_MAX),
	     {SDL_SCANCODE_W, SDL_CONTROLLER_AXIS_INVALID}},
		{CH(CH::KB_BRAKE, state, &RCS::brakes, RCS::CC_MAX),
	     {SDL_SCANCODE_S, SDL_CONTROLLER_AXIS_INVALID}},
		{CH(CH::KB_LEFT, state, &RCS::steering, RCS::CC_MIN, true),
	     {SDL_SCANCODE_A, SDL_CONTROLLER_AXIS_INVALID}},
		{CH(CH::KB_RIGHT, state, &RCS::steering, RCS::CC_MAX, true),
	     {SDL_SCANCODE_D, SDL_CONTROLLER_AXIS_INVALID}},

		// joystick-only inputs
		{CH(CH::GP_ACCEL, state, &RCS::acceleration),
	     {SDL_SCANCODE_UNKNOWN, SDL_CONTROLLER_AXIS_TRIGGERRIGHT}},
		{CH(CH::GP_BRAKE, state, &RCS::brakes),
	     {SDL_SCANCODE_UNKNOWN, SDL_CONTROLLER_AXIS_TRIGGERLEFT}},
		{CH(CH::GP_STEERING, state, &RCS::steering),
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

template <typename T>
void tryWrite(FILE* const file, const T* ptr, size_t size = sizeof(T),
              size_t count = 1) {
	auto written = fwrite(ptr, size, count, file);
	if (written != count) {
		throw std::runtime_error("Failed to write to file");
	}
}

void writeISM(FILE* const file, const InputSetupMap& ism) {
	for (const auto& [handler, inputs] : ism) {
		tryWrite(file, &handler.getID());
		SDL_Scancode key = std::get<SDL_Scancode>(inputs.first);
		tryWrite(file, &key);
		const auto& joystickInput = inputs.second;
		const bool isAxis =
			std::holds_alternative<SDL_GameControllerAxis>(joystickInput);
		tryWrite(file, &isAxis);
		if (isAxis) {
			SDL_GameControllerAxis axis =
				std::get<SDL_GameControllerAxis>(joystickInput);
			tryWrite(file, &axis);
		} else {
			SDL_GameControllerButton btn =
				std::get<SDL_GameControllerButton>(joystickInput);
			tryWrite(file, &btn);
		}
	}
}

template <typename T>
void tryRead(FILE* const file, T* value, size_t size = sizeof(T),
             size_t count = 1) {
	auto read = fread(value, size, count, file);
	if (read != count) {
		throw std::runtime_error("Failed to read");
	}
}

void readISM(FILE* const file, InputSetupMap& ism) {
	using HandlerID = RCState::ControlHandler::HandlerID;

	while (true) {
		static HandlerID id;
		static bool isAxis;
		static SDL_Scancode key;
		static SDL_GameControllerAxis axis;
		static SDL_GameControllerButton btn;
		ControlID joystick;

		try {
			tryRead(file, &id);
		} catch (...) {
			if (feof(file) != 0) {
				return;
			}
		}
		tryRead(file, &key);
		tryRead(file, &isAxis);
		if (isAxis) {
			tryRead(file, &axis);
			joystick = axis;
		} else {
			tryRead(file, &btn);
			joystick = btn;
		}
		for (auto& [handler, inputs] : ism) {
			if (handler.getID() == id) {
				inputs = std::make_pair(key, joystick);
				break;
			}
		}
	}
}
