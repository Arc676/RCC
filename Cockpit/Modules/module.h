#ifndef MODULE_H
#define MODULE_H

#include <cassert>
#include <cstddef>
#include <cstring>
#include <utility>

#include "Stream/netstream.h"
#include "interface.h"

class Module {
	bool cmdRequested = false;
	byte cmd[MESSAGE_BUFLEN];
	size_t cmdLength = 0;

protected:
	void setCmd(const byte* cmd, size_t len) {
		assert(len <= MESSAGE_BUFLEN);
		memcpy(this->cmd, cmd, len);
		cmdLength = len;
	}

	void setCmd(const byte cmd) {
		this->cmd[0] = cmd;
		cmdLength    = 1;
	}

	void requestCmd() { cmdRequested = true; }

public:
	virtual bool canHandleMessage(byte) const = 0;

	virtual void handleMessage(const byte*, size_t) = 0;

	virtual void render() = 0;

	bool shouldSendCmd() const { return cmdRequested; }

	std::pair<const byte*, size_t> getCmdData() {
		cmdRequested = false;
		return std::make_pair(cmd, cmdLength);
	}
};

#endif
