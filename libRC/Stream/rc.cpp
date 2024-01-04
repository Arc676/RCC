#include "rc.h"

using ControlHandler = RCState::ControlHandler;

ControlHandler::ControlHandler(RCState& state, CCPtr dst, bool isDoubleMapped)
	: state(state)
	, ccDst(dst)
	, isDoubleMapped(isDoubleMapped) {}

ControlHandler::ControlHandler(RCState& state, CCPtr dst, CC_t val,
                               bool isBidirectional, bool isDoubleMapped)
	: state(state)
	, ccDst(dst)
	, ccValue(val)
	, isBidirectional(isBidirectional)
	, isDoubleMapped(isDoubleMapped)
	, isDiscreteInput(true) {}

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
		if (!isDoubleMapped || valueIsSet()) {
			if (val == 1) {
				setValue();
			} else {
				unsetValue();
			}
		}
	} else {
		if (isContinuous) {
			state.*ccDst = val;
		}
	}
}
