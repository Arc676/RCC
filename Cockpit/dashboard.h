#ifndef DASHBOARD_H
#define DASHBOARD_H

#include <array>
#include <cstddef>
#include <string>
#include <thread>

#include "Modules/module.h"
#include "Modules/ping.h"
#include "Stream/netstream.h"
#include "interface.h"

class Dashboard {
	// singleton instance
	static Dashboard* instance;

	// connection state
	enum SocketStatus connectionStatus = DISCONNECTED;
	struct NetworkStream connection;
	char vehicleIP[IP_ADDR_BUFLEN] = {0};
	int vehiclePort                = DEFAULT_CONTROL_PORT;
	std::thread controlThread;

	// command modules
	static constexpr unsigned MODULE_COUNT = 1;
	PingModule ping;

	const std::array<Module*, MODULE_COUNT> modules = {&ping};

	// UI state
	bool showConnectionWindow = true;
	bool showCommandWindow    = false;

protected:
	bool menuBar();
	void connectionWindow();
	void commandHandler();
	void commandPanel();

	void handleCommand(const char*, size_t);

public:
	Dashboard() { Dashboard::instance = this; }

	bool drawCockpitUI();

	static Dashboard* getInstance() { return instance; }

	static void handler(const char* msg, size_t len) {
		getInstance()->handleCommand(msg, len);
	}
};

#endif
