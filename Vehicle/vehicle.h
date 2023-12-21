#ifndef VEHICLE_H
#define VEHICLE_H

#include <array>
#include <cstddef>

#include "Modules/modules.h"
#include "Modules/ping.h"
#include "Stream/netstream.h"
#include "Util/config.h"

class Vehicle : public MessageHandler {
	CmdlineArgs opts;
	NetworkStream controlStream;
	bool connected         = false;
	bool shutdownRequested = false;

	bool startupSuccessful = true;

	Ping pingReply;

protected:
	Responder* getResponder(byte);

public:
	Vehicle(int, char*[]);

	void run();

	void handleMessage(const byte*, size_t) override;

	bool shouldTerminate() const override { return !connected; };

	operator bool() const { return startupSuccessful; }
};

#endif
