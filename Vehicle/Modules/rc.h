#ifndef RC_MODULE_H
#define RC_MODULE_H

#include <cstddef>
#include <thread>

#include "Stream/netstream.h"
#include "Stream/rc.h"
#include "modules.h"

class RC : public Responder, public MessageHandler {
	NetworkStream stream;
	std::thread rcThread, acceptThread;
	bool stopStream = false;
	RCSetup setup;
	RCState state;

public:
	const char* name() const override { return "RC Stream"; }

	void respond(const byte*, size_t, struct Response&) override;

	bool shouldTerminate() const override { return stopStream; }

	void handleMessage(const byte*, size_t) override;
};

#endif
