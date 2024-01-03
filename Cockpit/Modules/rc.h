#ifndef RC_MODULE_H
#define RC_MODULE_H

#include <netinet/in.h>

#include <cstddef>

#include "Stream/netstream.h"
#include "Stream/rc.h"
#include "interface.h"
#include "module.h"

class RCModule : public Module {
	NetworkStream stream;
	enum SocketStatus result = DISCONNECTED;
	RCSetup setup;

public:
	bool canHandleMessage(const byte cmd) const override {
		return cmd == RC_OK || cmd == RC_ERROR;
	}

	void handleMessage(const byte*, size_t) override;

	void render() override;
};

#endif
