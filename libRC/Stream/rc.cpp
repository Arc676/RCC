#include "rc.h"

#include <compare>
#include <string>

using ControlHandler = RCState::ControlHandler;

ControlHandler::ControlHandler(const std::string& name, RCState& state,
                               CCPtr dst)
	: state(state)
	, name(name)
	, ccDst(dst) {}

ControlHandler::ControlHandler(const std::string& name, RCState& state,
                               CCPtr dst, CC_t val, bool isDoubleMapped)
	: ControlHandler(name, state, dst) {
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

std::weak_ordering ControlHandler::operator<=>(
	const ControlHandler& other) const {
	return name <=> other.name;
}
