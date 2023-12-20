#ifndef MODULE_H
#define MODULE_H

#include <cassert>
#include <cstddef>
#include <cstring>
#include <utility>

#include "Stream/netstream.h"

class Module {
	bool cmdRequested = false;
	char cmd[MESSAGE_BUFLEN];
	size_t cmdLength = 0;

protected:
	void setCmd(const char* cmd, size_t len) {
		assert(len <= MESSAGE_BUFLEN);
		memcpy(this->cmd, cmd, len);
		cmdLength = len;
	}

	void setCmd(const char cmd) {
		this->cmd[0] = cmd;
		cmdLength    = 1;
	}

	void requestCmd() { cmdRequested = true; }

public:
	virtual bool canHandleMessage(char) const = 0;

	virtual void handleMessage(const char*, size_t) = 0;

	virtual void render() = 0;

	bool shouldSendCmd() const { return cmdRequested; }

	std::pair<const char*, size_t> getCmdData() {
		cmdRequested = false;
		return std::make_pair(cmd, cmdLength);
	}
};

#endif
