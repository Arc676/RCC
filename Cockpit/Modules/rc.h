#ifndef RC_MODULE_H
#define RC_MODULE_H

#include <SDL_events.h>
#include <SDL_gamecontroller.h>
#include <SDL_joystick.h>
#include <SDL_keycode.h>
#include <netinet/in.h>

#include <cstddef>
#include <cstdint>
#include <map>
#include <thread>

#include "Controls/input.h"
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

	bool lastReadOK = true;

	InputSetupMap ism;
	InputMap controls;
	RCState state;

	bool showControlSetup = false;
	struct {
		bool active = false;
		bool keyboard;
		InputSetupMap::iterator it;
	} listener;

	SDL_GameController* joystick = nullptr;
	SDL_JoystickID joystickID    = -1;
	int selectedJoystick         = -1;
	const char* joystickError    = nullptr;
	struct {
		const char* name = nullptr;
		const char* ctrl1;
		const char* ctrl2;

		void clear() { name = ctrl1 = ctrl2 = nullptr; }

		bool exists() const { return name != nullptr; }
	} duplicateInput;

	const char* joystickName() const;

	void joystickSelect();

	void showControls(bool keyboard);

	void changeControls();

	bool isDuplicateInput(bool, const ControlID&);

	bool interceptInput(const SDL_Event*);

	void streamSetup();

	void transmissionControls();

	void setTransmissionDelay();

	void renderSocketStates() const;

	void startTransmitting();

	void stopTransmitting();

public:
	/**
	 * @brief Set up the RC module with the default controls defined in
	 * Modules/default_controls.cpp
	 *
	 * @param dash Owning dashboard
	 */
	RCModule(const Dashboard* dash)
		: Module(dash) {
		ism = getDefaultInputs(state);
	}

	bool canHandleMessage(const byte cmd) const override {
		return cmd == RC_OK || cmd == RC_ERROR;
	}

	void transmitLoop() const;

	bool handlesEvents() const override { return true; }

	void handleEvent(const SDL_Event*) override;

	void handleMessage(ConstBuf&) override;

	void render() override;
};

#endif
