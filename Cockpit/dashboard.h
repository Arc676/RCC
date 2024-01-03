#ifndef DASHBOARD_H
#define DASHBOARD_H

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
	static std::array<std::unique_ptr<Module>, sizeof...(Modules)>
	makeModules() {
		return {std::make_unique<Modules>()...};
	}
#define MAKE_MODULES(name, ...)                       \
	const decltype(makeModules<__VA_ARGS__>()) name = \
		makeModules<__VA_ARGS__>();
	MAKE_MODULES(modules, PingModule, CameraModule)

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
	bool drawCockpitUI();

	void handleMessage(const byte*, size_t) override;

	bool shouldTerminate() const override {
		return connectionStatus != SOCKET_OK;
	}
};

#endif
