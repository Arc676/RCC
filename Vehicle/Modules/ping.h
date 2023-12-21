#ifndef PING_MODULE_H
#define PING_MODULE_H

#include <cstddef>

#include "modules.h"

class Ping : public Responder {
public:
	void respond(const byte*, size_t, struct Response&) override;
};

#endif
