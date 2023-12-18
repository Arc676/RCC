#include <string>

#define IP_ADDR_BUFLEN 30

class Dashboard {
	// connection state
	bool connected = false;
	char vehicleIP[IP_ADDR_BUFLEN];
	int vehiclePort;

	// UI state
	bool showConnectionWindow = true;

protected:
	bool menuBar();
	void connectionWindow();

public:
	bool drawCockpitUI();
};