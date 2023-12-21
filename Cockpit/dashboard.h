#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <array>
#include <cstddef>
#include <string>
#include <thread>

#include "Modules/camera.h"
#include "Modules/module.h"
#include "Modules/ping.h"
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
	static constexpr unsigned MODULE_COUNT = 2;
	PingModule ping;
	CameraModule cam;

	const std::array<Module*, MODULE_COUNT> modules = {&ping, &cam};

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
