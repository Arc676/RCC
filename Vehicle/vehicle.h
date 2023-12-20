#ifndef VEHICLE_H
#define VEHICLE_H

#include <cstddef>

#include "Stream/netstream.h"
#include "Util/config.h"

class Vehicle {
	CmdlineArgs opts;
	struct NetworkStream controlStream;
	bool connected         = false;
	bool shutdownRequested = false;

	bool startupSuccessful = true;

	static Vehicle* instance;

	void handler(const byte*, size_t);

public:
	Vehicle(int, char*[]);

	static void handle(const byte* msg, size_t len) {
		instance->handler(msg, len);
	}

	void run();

	operator bool() const { return startupSuccessful; }
};

#endif
