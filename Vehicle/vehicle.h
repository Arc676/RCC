#ifndef VEHICLE_H
#define VEHICLE_H

#include <cstddef>

#include "Stream/netstream.h"
#include "Util/config.h"

class Vehicle : public MessageHandler {
	CmdlineArgs opts;
	NetworkStream controlStream;
	bool connected         = false;
	bool shutdownRequested = false;

	bool startupSuccessful = true;

public:
	Vehicle(int, char*[]);

	void run();

	void handleMessage(const byte*, size_t) override;

	bool shouldTerminate() const override { return !connected; };

	operator bool() const { return startupSuccessful; }
};

#endif
