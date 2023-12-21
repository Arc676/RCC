#ifndef VEHICLE_MODULES_H
#define VEHICLE_MODULES_H

#include <stddef.h>

#include "Stream/netstream.h"
#include "interface.h"

struct Response {
	byte data[MESSAGE_BUFLEN];
	size_t len;
};

class Responder {
public:
	virtual void respond(const byte*, size_t, struct Response&) = 0;
};

#endif
