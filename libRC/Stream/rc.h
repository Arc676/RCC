#ifndef RC_STREAM_H
#define RC_STREAM_H

#include <netinet/in.h>

#include <cstdint>

#include "interface.h"

struct RCSetup {
	bool running = false;
	int port     = DEFAULT_RC_PORT;
	int protocol = IPPROTO_UDP;
};

struct RCState {
	// Continuous control type
	using CC_t = float;

	constexpr static CC_t CC_MAX     = 1;
	constexpr static CC_t CC_NEUTRAL = 0;
	constexpr static CC_t CC_MIN     = -1;

	CC_t acceleration;
	CC_t brakes;
	CC_t steering;

	class ControlHandler {
		bool isBidirectional = false;
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
		ControlHandler(RCState& state, CCPtr dst, bool isDoubleMapped = false);

		ControlHandler(RCState& state, CCPtr dst, CC_t val,
		               bool isBidirectional, bool isDoubleMapped = false);

		void operator()(const float& val) const;
	};
};

#endif
