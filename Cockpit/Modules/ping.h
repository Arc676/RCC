#ifndef PING_MODULE_H
#define PING_MODULE_H

#include <chrono>
#include <cstddef>

#include "interface.h"
#include "module.h"

class PingModule : public Module {
	std::chrono::microseconds latency = std::chrono::microseconds(0);
	byte response                     = 0;

	/**
	 * @brief Decode the last ping response (or an internally set flag)
	 *
	 * @return Human-readable representation of the vehicle's ping response
	 */
	const char* decodeResponse() const;

public:
	PingModule(const Dashboard* dash)
		: Module(dash) {
		getCmdBuffer() << PING;
	}

	bool canHandleMessage(const byte opCode) const override {
		return opCode == PING_ACK;
	}

	void handleMessage(ConstBuf&) override;

	void render() override;
};

#endif
