#ifndef DRIVER_INTERFACE_H
#define DRIVER_INTERFACE_H

#include <memory>

#include "Stream/rc.h"

class Driver {
	const RCState* state = nullptr;

	void cacheState(const RCState& state) { this->state = &state; }

public:
	Driver()                         = default;
	virtual ~Driver()                = default;
	Driver(const Driver&)            = delete;
	Driver(Driver&&)                 = default;
	Driver& operator=(const Driver&) = delete;
	Driver& operator=(Driver&&)      = default;

	/**
	 * @brief Update the vehicle based on the input controls
	 *
	 * @param state Controller input state
	 */
	virtual void update(const RCState& state) = 0;
};

/**
 * @brief Create a Driver for the vehicle
 *
 * @param argc Argument count
 * @param argv Argument vector
 * @return Shared pointer to the new driver
 */
extern std::shared_ptr<Driver> createDriver(int argc, char** argv);

#endif
