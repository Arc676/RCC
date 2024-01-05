#ifndef DRIVER_INTERFACE_H
#define DRIVER_INTERFACE_H

#include "Stream/rc.h"

/**
 * @brief Update the vehicle based on the input controls
 *
 * @param state Controller input state
 */
extern void update(const RCState& state);

#endif
