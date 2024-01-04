#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <SDL_events.h>

#include <array>
#include <cstddef>
#include <memory>
#include <string>
#include <thread>

#include "Modules/camera.h"
#include "Modules/module.h"
#include "Modules/ping.h"
#include "Modules/rc.h"
#include "Stream/netstream.h"
#include "interface.h"

class Dashboard : public MessageHandler {
	// connection state
	enum SocketStatus connectionStatus = DISCONNECTED;
	NetworkStream connection;
	char vehicleIP[IP_ADDR_BUFLEN] = {0};
	int vehiclePort                = DEFAULT_CONTROL_PORT;
	std::thread controlThread;

	// command modules
	template <typename... Modules>
	static std::array<std::unique_ptr<Module>, sizeof...(Modules)> makeModules(
		const Dashboard* dash) {
		return {std::make_unique<Modules>(dash)...};
	}
#define MODULES PingModule, CameraModule, RCModule
	const decltype(makeModules<MODULES>(nullptr)) modules;

	// UI state
	bool showConnectionWindow = true;
	bool showCommandWindow    = false;

protected:
	bool menuBar();
	void connectionWindow();
	void commandHandler();
	void commandPanel();

	void disconnect();

public:
	Dashboard()
		: modules(makeModules<MODULES>(this)) {}

	bool drawCockpitUI();

	void handleEvent(const SDL_Event* event);

	void handleMessage(const byte*, size_t) override;

	bool shouldTerminate() const override {
		return connectionStatus != SOCKET_OK;
	}
};

#endif
