#ifndef RC_STREAM_H
#define RC_STREAM_H

#include <netinet/in.h>

#include <cstdint>
#include <string>

#include "interface.h"

struct RCSetup {
	bool running = false;
	int port     = DEFAULT_RC_PORT;
	int protocol = IPPROTO_UDP;
};

struct __attribute__((packed)) RCState {
	// Continuous control type
	using CC_t = float;

	constexpr static CC_t CC_MAX     = 1;
	constexpr static CC_t CC_NEUTRAL = 0;
	constexpr static CC_t CC_MIN     = -1;

	CC_t acceleration = CC_NEUTRAL;
	CC_t brakes       = CC_NEUTRAL;
	CC_t steering     = CC_NEUTRAL;

	class ControlHandler {
		const std::string name;

		bool isDoubleMapped  = false;
		bool isDiscreteInput = false;
		bool isContinuous    = true;

		using CCPtr = CC_t RCState::*;

		RCState& state;

		union {
			CCPtr ccDst;
		};
		union {
			CC_t ccValue;
		};

		void setValue() const;

		void unsetValue() const;

		bool valueIsSet() const;

	public:
		ControlHandler(const std::string& name, RCState& state, CCPtr dst,
		               CC_t val = CC_MAX, bool isDoubleMapped = false);

		void operator()(const float& val) const;

		const std::string& getName() const { return name; }
	};
};

#endif
