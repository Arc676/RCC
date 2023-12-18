#include <string>

#include "Modules/ping.h"
#include "interface.h"

#define IP_ADDR_BUFLEN 30

class Dashboard {
	// connection state
	bool connected                 = false;
	char vehicleIP[IP_ADDR_BUFLEN] = {0};
	int vehiclePort                = DEFAULT_CONTROL_PORT;

	// command modules
	PingModule ping;

	// UI state
	bool showConnectionWindow = true;
	bool showCommandWindow    = true;

protected:
	bool menuBar();
	void connectionWindow();
	void commandPanel();

public:
	bool drawCockpitUI();
};
