#ifndef MODULE_H
#define MODULE_H

#include <SDL_events.h>

#include <cassert>
#include <cstddef>
#include <cstring>
#include <utility>

#include "Stream/buffer.h"
#include "Stream/netstream.h"
#include "interface.h"

class Dashboard;

class Module {
protected:
	using Buf = Buffer<byte>;

private:
	bool cmdRequested = false;
	Buf cmd;
	size_t cmdLength = 0;

	const Dashboard* dash;

protected:
	using ConstBuf = Buffer<const byte>;

	/**
	 * @brief Get the dashboard object
	 *
	 * @return Pointer to dashboard
	 */
	[[nodiscard]] const Dashboard* getDashboard() const { return dash; }

	/**
	 * @brief Get the command buffer
	 *
	 * @return Command buffer
	 */
	Buf& getCmdBuffer() { return cmd; }

	/**
	 * @brief Get the command buffer after clearing it
	 *
	 * @return Command buffer
	 */
	Buf& getCleanCmdBuffer() {
		cmd.clear();
		return getCmdBuffer();
	}

	/**
	 * @brief Requests a single byte command
	 *
	 * @param cmd Command to send
	 */
	void requestCmd(const byte cmd) {
		getCleanCmdBuffer() << cmd;
		requestCmd();
	}

	/**
	 * @brief Indicate that the provided command should be sent to the vehicle
	 */
	void requestCmd() { cmdRequested = true; }

public:
	virtual ~Module()                = default;
	Module(const Module&)            = default;
	Module(Module&&)                 = delete;
	Module& operator=(const Module&) = delete;
	Module& operator=(Module&&)      = delete;

	/**
	 * @brief Construct a new module
	 *
	 * @param dash The dashboard from which modules can retrieve relevant
	 * information
	 */
	explicit Module(const Dashboard* dash)
		: dash(dash)
		, cmd(MESSAGE_BUFLEN) {}

	/**
	 * @brief Determine whether this module is set up to handle a given response
	 *
	 * @param opCode The reply opcode
	 * @return Whether this module should handle the message
	 */
	[[nodiscard]] virtual bool canHandleMessage(byte opCode) const = 0;

	/**
	 * @brief Handle an incoming message
	 *
	 * @param msg Message buffer
	 */
	virtual void handleMessage(ConstBuf& msg) = 0;

	/**
	 * @brief Determine whether this module should be used to handle SDL events
	 *
	 * @return Whether the module handles events
	 */
	[[nodiscard]] virtual bool handlesEvents() const { return false; }

	/**
	 * @brief Handle an SDL event
	 *
	 * @param event The SDL event
	 */
	virtual void handleEvent(const SDL_Event* event) {}

	/**
	 * @brief Render the module UI
	 */
	virtual void render() = 0;

	/**
	 * @brief Has a command been requested?
	 *
	 * @return Whether the module command should be sent
	 */
	[[nodiscard]] bool shouldSendCmd() const { return cmdRequested; }

	/**
	 * @brief Get the command to be sent and mark the module as no longer
	 * requiring that its command be sent
	 *
	 * @return The command buffer and its length as a std::pair
	 */
	const Buf& getCmdData() {
		cmdRequested = false;
		return cmd;
	}
};

#endif
