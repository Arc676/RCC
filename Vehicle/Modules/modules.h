#ifndef VEHICLE_MODULES_H
#define VEHICLE_MODULES_H

#include <cstddef>

#include "Stream/netstream.h"
#include "interface.h"

/**
 * @brief A response to an incoming command
 */
struct Response {
	byte data[MESSAGE_BUFLEN];
	size_t len;
};

class Responder {
public:
	/**
	 * @brief Respond to an incoming command
	 *
	 * @param msg Command buffer
	 * @param len Command length
	 * @param response Response object in which to construct reply message, if
	 * any
	 */
	virtual void respond(const byte* msg, size_t len,
	                     struct Response& response) = 0;
};

#endif
