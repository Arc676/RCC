#ifndef RC_MODULE_H
#define RC_MODULE_H

#include <SDL_events.h>
#include <SDL_joystick.h>
#include <SDL_keycode.h>
#include <netinet/in.h>

#include <cstddef>
#include <cstdint>
#include <map>
#include <thread>

#include "Stream/netstream.h"
#include "Stream/rc.h"
#include "interface.h"
#include "module.h"

class RCModule : public Module {
	constexpr static int DEFAULT_TRANSMISSION_DELAY = 100;
	constexpr static int MAX_TRANSMISSION_DELAY     = 1000;

	NetworkStream stream;
	enum SocketStatus remoteSockState = DISCONNECTED;
	enum SocketStatus localSockState  = DISCONNECTED;
	std::thread transmitThread;
	int transmitDelay = DEFAULT_TRANSMISSION_DELAY;
	RCSetup setup;

	enum ControllerType {
		KEYBOARD,
		JOYSTICK,
	};

	using ControlID     = uint32_t;
	using InputMap      = std::map<ControlID, RCState::ControlHandler>;
	using ControllerMap = std::map<ControllerType, InputMap>;
	ControllerMap controls;
	RCState state;

	void transmissionControls();

	void setTransmissionDelay();

	void renderSocketStates() const;

public:
	/**
	 * @brief Set up the RC module with the default controls defined in
	 * Modules/default_controls.cpp
	 */
	RCModule();

	bool canHandleMessage(const byte cmd) const override {
		return cmd == RC_OK || cmd == RC_ERROR;
	}

	void transmitLoop() const;

	bool handlesEvents() const override { return true; }

	void handleEvent(const SDL_Event*) override;

	void handleMessage(const byte*, size_t) override;

	void render() override;
};

#endif
