#include <string>

#include "interface.h"

#define IP_ADDR_BUFLEN 30

class Dashboard {
	// connection state
	bool connected                 = false;
	char vehicleIP[IP_ADDR_BUFLEN] = {0};
	int vehiclePort                = DEFAULT_CONTROL_PORT;

	// UI state
	bool showConnectionWindow = true;

protected:
	bool menuBar();
	void connectionWindow();

public:
	bool drawCockpitUI();
};