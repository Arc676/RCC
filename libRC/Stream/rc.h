#ifndef RC_STREAM_H
#define RC_STREAM_H

#include <netinet/in.h>

#include <compare>
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
	public:
		enum HandlerID {
			KB_ACCEL,
			KB_BRAKE,
			KB_LEFT,
			KB_RIGHT,

			GP_ACCEL,
			GP_BRAKE,
			GP_STEERING,
		};

	private:
		HandlerID id;

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
		ControlHandler(HandlerID id, RCState& state, CCPtr dst, CC_t val,
		               bool isDoubleMapped = false);

		ControlHandler(HandlerID id, RCState& state, CCPtr dst);

		void operator()(const float& val) const;

		const char* getName() const;

		std::strong_ordering operator<=>(const ControlHandler& other) const;
	};
};

#endif
