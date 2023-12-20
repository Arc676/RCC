#ifndef PING_MODULE_H
#define PING_MODULE_H

#include <chrono>
#include <cstddef>

#include "interface.h"
#include "module.h"

class PingModule : public Module {
	std::chrono::microseconds latency = std::chrono::microseconds(0);
	unsigned char response            = 0;

	const char* decodeResponse() const;

public:
	PingModule() { setCmd(PING); }

	bool canHandleMessage(const char opCode) const override {
		return opCode == PING_ACK;
	}

	void handleMessage(const char* msg, size_t len) override;

	void render() override;
};

#endif