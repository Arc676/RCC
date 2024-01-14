#ifndef DRIVER_INTERFACE_H
#define DRIVER_INTERFACE_H

#include "Stream/rc.h"

class Driver {
	const RCState* state = nullptr;

	void cacheState(const RCState& state) { this->state = &state; }

public:
	/**
	 * @brief Construct a new Driver from command line arguments
	 *
	 * @param argc Argument count
	 * @param argv Argument vector
	 */
	Driver(int argc, char** argv) {}

	/**
	 * @brief Update the vehicle based on the input controls
	 *
	 * @param state Controller input state
	 */
	void update(const RCState& state);
};

#endif
