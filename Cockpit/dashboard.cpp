#include "dashboard.h"

#include <netinet/in.h>

#include "Stream/netstream.h"
#include "imgui.h"

bool Dashboard::menuBar() {
	ImGui::BeginMainMenuBar();

	if (ImGui::BeginMenu("File")) {
		if (connectionStatus == SOCKET_OK) {
			if (ImGui::MenuItem("Disconnect from vehicle")) {
				// TODO
			}
		} else {
			if (ImGui::MenuItem("Connect to vehicle")) {
				showConnectionWindow = true;
			}
		}
		if (ImGui::MenuItem("Show command panel")) {
			showCommandWindow = true;
		}
		if (ImGui::MenuItem("Quit")) {
			return true;
		}
		ImGui::EndMenu();
	}
	ImGui::EndMainMenuBar();
	return false;
}

void Dashboard::connectionWindow() {
	if (ImGui::Begin("Connection", &showConnectionWindow)) {
		ImGui::InputText("Vehicle IP Address", vehicleIP, IP_ADDR_BUFLEN);
		ImGui::InputInt("Control Post", &vehiclePort);
		ImGui::Text("Status: %s", getSocketError(connectionStatus));

		if (connectionStatus == DISCONNECTED && ImGui::Button("Connect")) {
			connectionStatus = netstream_initClient(&connection, vehicleIP,
			                                        vehiclePort, IPPROTO_TCP);
		} else if (connectionStatus == SOCKET_OK
		           && ImGui::Button("Disconnect")) {
			netstream_disconnect(&connection);
			connectionStatus = DISCONNECTED;
		}

		if (connectionStatus != DISCONNECTED && connectionStatus != SOCKET_OK
		    && ImGui::Button("OK")) {
			connectionStatus = DISCONNECTED;
		}
	}
	ImGui::End();
}

void Dashboard::commandPanel() {
	if (ImGui::Begin("Command Panel", &showCommandWindow)) {
		ping.render();
	}
	ImGui::End();
}

bool Dashboard::drawCockpitUI() {
	if (menuBar()) {
		return true;
	}
	if (showConnectionWindow) {
		connectionWindow();
	}
	if (showCommandWindow) {
		commandPanel();
	}
	return false;
}
