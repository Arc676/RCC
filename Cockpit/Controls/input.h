#ifndef INPUT_H
#define INPUT_H

#include <SDL_events.h>
#include <SDL_gamecontroller.h>
#include <SDL_scancode.h>
#include <bits/types/FILE.h>

#include <istream>
#include <map>
#include <ostream>
#include <utility>
#include <variant>

#include "Stream/rc.h"

/**
 * @brief Accept key presses, joystick motion, or gamepad buttons as input
 */
using ControlID = std::variant<SDL_Scancode, SDL_GameControllerAxis,
                               SDL_GameControllerButton>;
/**
 * @brief Map controls to keyboard and gamepad inputs; used when the user is
 * setting up the input map
 */
using InputSetupMap =
	std::map<RCState::ControlHandler, std::pair<ControlID, ControlID>>;
/**
 * @brief Maps inputs to controls; used for quickly handling user input when
 * sending controls to the vehicle
 *
 */
using InputMap = std::map<ControlID, RCState::ControlHandler>;

/**
 * @brief Generate default input setup map
 *
 * @param state
 * @return InputSetupMap
 */
InputSetupMap getDefaultInputs(RCState& state);

/**
 * @brief Check if an input identifier is valid, i.e. it's neither the
 * placeholder value for unknown scancodes, invalid joystick axes, nor invalid
 * gamepad buttons.
 *
 * @param cid Input identifier
 * @return Identifier validity
 */
bool isValidMapping(const ControlID& cid);

/**
 * @brief Get the human readable form of an input identifier
 *
 * @param cid Input identifier
 * @return Name of the input
 */
const char* getInputName(const ControlID& cid);

/**
 * @brief Creates an input map for a given input setup map; this is effectively
 * a new map with the keys and values swapped and duplicated (i.e. the same
 * control appears once for a keyboard input and once for a gamepad input)
 *
 * @param ism Input setup map
 * @return An input map described by the setup map
 */
InputMap createInputMap(const InputSetupMap& ism);

/**
 * @brief Writes the input setup map to disk for later retrieval
 *
 * @param file Output file
 * @param ism Input setup map
 */
void writeISM(FILE* file, const InputSetupMap& ism);

/**
 * @brief Reads the input setup map from disk
 *
 * @param file Input file
 * @param ism Input setup map to restore from disk
 */
void readISM(FILE* file, InputSetupMap& ism);

#endif
