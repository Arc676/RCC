#include "rc.h"

#include <compare>
#include <string>

using ControlHandler = RCState::ControlHandler;

ControlHandler::ControlHandler(HandlerID id, RCState& state, CCPtr dst)
	: state(state)
	, id(id)
	, ccDst(dst) {}

ControlHandler::ControlHandler(HandlerID id, RCState& state, CCPtr dst,
                               CC_t val, bool isDoubleMapped)
	: ControlHandler(id, state, dst) {
	ccValue              = val;
	this->isDoubleMapped = isDoubleMapped;
	isDiscreteInput      = true;
}

void ControlHandler::setValue() const {
	if (isContinuous) {
		state.*ccDst = ccValue;
	}
}

void ControlHandler::unsetValue() const {
	if (isContinuous) {
		state.*ccDst = CC_NEUTRAL;
	}
}

bool ControlHandler::valueIsSet() const {
	if (isContinuous) {
		return state.*ccDst == ccValue;
	}
	return false;
}

void ControlHandler::operator()(const float& val) const {
	if (isDiscreteInput) {
		if (val == 1) {
			setValue();
		} else if (!isDoubleMapped || valueIsSet()) {
			unsetValue();
		}
	} else {
		if (isContinuous) {
			state.*ccDst = val;
		}
	}
}

std::strong_ordering ControlHandler::operator<=>(
	const ControlHandler& other) const {
	return id <=> other.id;
}

const char* ControlHandler::getName() const {
	switch (id) {
		case KB_ACCEL:
		case GP_ACCEL:
			return "Accelerate";
		case KB_BRAKE:
		case GP_BRAKE:
			return "Brake";
		case KB_LEFT:
			return "Turn left";
		case KB_RIGHT:
			return "Turn right";
		case GP_STEERING:
			return "Steering";
		default:
			return "Unknown control";
	}
}
