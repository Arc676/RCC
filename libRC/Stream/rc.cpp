#include "rc.h"

#include <compare>
#include <string>
#include <variant>

using ControlHandler = RCState::ControlHandler;

ControlHandler::ControlHandler(HandlerID id, RCState& state, Ptr dst)
	: state(state)
	, id(id)
	, dst(dst) {}

ControlHandler::ControlHandler(HandlerID id, RCState& state, Ptr dst, Value val,
                               bool isDoubleMapped)
	: ControlHandler(id, state, dst) {
	// NOLINTBEGIN(*member-initializer)
	value                = val;
	this->isDoubleMapped = isDoubleMapped;
	isDiscreteInput      = true;
	// NOLINTEND(*member-initializer)
}

void ControlHandler::setValue() const {
	if (isContinuous) {
		state.*std::get<CCPtr>(dst) = std::get<CC_t>(value);
	}
}

void ControlHandler::unsetValue() const {
	if (isContinuous) {
		state.*std::get<CCPtr>(dst) = CC_NEUTRAL;
	}
}

bool ControlHandler::valueIsSet() const {
	if (isContinuous) {
		return state.*std::get<CCPtr>(dst) == std::get<CC_t>(value);
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
			state.*std::get<CCPtr>(dst) = val;
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
