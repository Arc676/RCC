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
	/**
	 * @brief Set the command to be sent
	 *
	 * @param cmd Command buffer
	 * @param len Command length
	 */
	void setCmd(const byte* cmd, size_t len) {
		assert(len <= MESSAGE_BUFLEN);
		memcpy(this->cmd, cmd, len);
		cmdLength = len;
	}

	/**
	 * @brief Set a single byte command
	 *
	 * @param cmd Command opcode
	 */
	void setCmd(const byte cmd) {
		this->cmd[0] = cmd;
		cmdLength    = 1;
	}

	/**
	 * @brief Indicate that the provided command should be sent to the vehicle
	 */
	void requestCmd() { cmdRequested = true; }

public:
	/**
	 * @brief Determine whether this module is set up to handle a given response
	 *
	 * @param opCode The reply opcode
	 * @return Whether this module should handle the message
	 */
	virtual bool canHandleMessage(byte opCode) const = 0;

	/**
	 * @brief Handle an incoming message
	 *
	 * @param msg Message buffer
	 * @param len Message length
	 */
	virtual void handleMessage(const byte* msg, size_t len) = 0;

	/**
	 * @brief Render the module UI
	 */
	virtual void render() = 0;

	/**
	 * @brief Has a command been requested?
	 *
	 * @return Whether the module command should be sent
	 */
	bool shouldSendCmd() const { return cmdRequested; }

	/**
	 * @brief Get the command to be sent and mark the module as no longer
	 * requiring that its command be sent
	 *
	 * @return The command buffer and its length as a std::pair
	 */
	std::pair<const byte*, size_t> getCmdData() {
		cmdRequested = false;
		return std::make_pair(cmd, cmdLength);
	}
};

#endif
