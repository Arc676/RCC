#include "dashboard.h"

#include <netinet/in.h>

#include <cstddef>
#include <cstdio>
#include <thread>

#include "Stream/netstream.h"
#include "imgui.h"

Dashboard* Dashboard::instance = nullptr;

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

void Dashboard::handleCommand(const char* msg, size_t len) {
	for (auto* module : modules) {
		if (module->canHandleMessage(msg[0])) {
			module->handleMessage(msg, len);
		}
	}
}

void Dashboard::connectionWindow() {
	if (ImGui::Begin("Connection", &showConnectionWindow)) {
		ImGui::InputText("Vehicle IP Address", vehicleIP, IP_ADDR_BUFLEN);
		ImGui::InputInt("Control Post", &vehiclePort);
		ImGui::Text("Status: %s", getSocketError(connectionStatus));

		if (connectionStatus == DISCONNECTED && ImGui::Button("Connect")) {
			connectionStatus = netstream_initClient(&connection, vehicleIP,
			                                        vehiclePort, IPPROTO_TCP);
			if (connectionStatus == SOCKET_OK) {
				controlThread = std::thread(netstream_recvLoop, &connection,
				                            Dashboard::handler, &vehiclePort);
			}
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
		for (auto* module : modules) {
			module->render();
		}
	}
	ImGui::End();
}

void Dashboard::commandHandler() {
	if (showCommandWindow) {
		commandPanel();
	}
	for (auto* module : modules) {
		if (module->shouldSendCmd()) {
			auto data = module->getCmdData();
			netstream_send(&connection, data.first, data.second);
		}
	}
}

bool Dashboard::drawCockpitUI() {
	if (menuBar()) {
		return true;
	}
	if (showConnectionWindow) {
		connectionWindow();
	}
	commandHandler();
	return false;
}
