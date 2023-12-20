#ifndef PING_MODULE_H
#define PING_MODULE_H

#include <cstddef>

#include "interface.h"
#include "module.h"

class PingModule : public Module {
	long latency  = 0;
	char response = 0;

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
