#ifndef RC_MODULE_H
#define RC_MODULE_H

#include <cstddef>
#include <memory>
#include <thread>
#include <utility>

#include "Drivers/interface.h"
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

	std::shared_ptr<Driver> driver;

	enum SocketStatus setupStream();

	void stopStream();

public:
	explicit RC(std::shared_ptr<Driver> driver)
		: driver(std::move(driver)) {}
	~RC() override;

	RC(const RC&)            = delete;
	RC(RC&&)                 = default;
	RC& operator=(const RC&) = delete;
	RC& operator=(RC&&)      = default;

	[[nodiscard]] const char* name() const override { return "RC Stream"; }

	bool respond(ConstBuf& msg, Buf& response) override;

	[[nodiscard]] bool shouldTerminate() const override {
		return stopReceiving;
	}

	void handleMessage(const byte* msg, size_t len) override;
};

#endif
