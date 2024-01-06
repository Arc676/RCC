#ifndef PING_MODULE_H
#define PING_MODULE_H

#include <cstddef>

#include "modules.h"

class Ping : public Responder {
public:
	const char* name() const override { return "Ping"; }

	void respond(Buf&, struct Response&) override;
};

#endif
