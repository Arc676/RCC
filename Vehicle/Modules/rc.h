#ifndef RC_MODULE_H
#define RC_MODULE_H

#include <cstddef>
#include <thread>

#include "Stream/netstream.h"
#include "Stream/rc.h"
#include "modules.h"

class RC : public Responder, public MessageHandler {
	NetworkStream stream;
	bool stopAccepting = false;
	bool stopReceiving = false;
	std::thread rcThread;

	RCSetup setup;
	RCState state;

	enum SocketStatus setupStream();

	void stopStream();

public:
	~RC();

	[[nodiscard]] const char* name() const override { return "RC Stream"; }

	bool respond(ConstBuf& msg, Buf& response) override;

	[[nodiscard]] bool shouldTerminate() const override {
		return stopReceiving;
	}

	void handleMessage(const byte* msg, size_t len) override;
};

#endif
