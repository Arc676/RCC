#ifndef INPUT_H
#define INPUT_H

#include <SDL_events.h>
#include <SDL_gamecontroller.h>
#include <SDL_scancode.h>

#include <map>
#include <utility>
#include <variant>

#include "Stream/rc.h"

using ControlID = std::variant<SDL_Scancode, SDL_GameControllerAxis,
                               SDL_GameControllerButton>;
using InputSetupMap =
	std::map<RCState::ControlHandler, std::pair<ControlID, ControlID>>;
using InputMap = std::map<ControlID, RCState::ControlHandler>;

InputSetupMap getDefaultInputs(RCState& state);

bool isValidMapping(const ControlID& cid);

const char* getInputName(const ControlID& cid);

InputMap createInputMap(const InputSetupMap& ism);

#endif
