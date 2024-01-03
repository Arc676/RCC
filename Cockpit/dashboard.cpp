#include "dashboard.h"

#include <netinet/in.h>

#include <cstddef>
#include <cstdio>
#include <thread>

#include "Stream/netstream.h"
#include "imgui.h"
#include "interface.h"

bool Dashboard::menuBar() {
	ImGui::BeginMainMenuBar();

	if (ImGui::BeginMenu("Cockpit")) {
		if (connectionStatus == SOCKET_OK) {
			if (ImGui::MenuItem("Disconnect from vehicle")) {
				disconnect();
			}
		} else {
			if (ImGui::MenuItem("Connect to vehicle")) {
				showConnectionWindow = true;
			}
		}
		if (ImGui::MenuItem("Quit")) {
			return true;
		}
		ImGui::EndMenu();
	}
	if (connectionStatus == SOCKET_OK && ImGui::BeginMenu("Commands")) {
		if (ImGui::MenuItem("Show command panel")) {
			showCommandWindow = true;
		}
		if (ImGui::MenuItem("Shut down vehicle")) {
			static const byte SHUTDOWN_CMD = SHUTDOWN;
			connection.send(&SHUTDOWN_CMD, 1);
			disconnect();
		}
		ImGui::EndMenu();
	}
	ImGui::EndMainMenuBar();
	return false;
}

void Dashboard::disconnect() {
	connection.disconnect();
	connectionStatus  = DISCONNECTED;
	showCommandWindow = false;
	controlThread.join();
}

void Dashboard::handleEvent(const SDL_Event* const event) {
	for (const auto& module : modules) {
		if (module->handlesEvents()) {
			module->handleEvent(event);
		}
	}
}

void Dashboard::handleMessage(const byte* msg, size_t len) {
	for (const auto& module : modules) {
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
			connectionStatus =
				connection.initClient(vehicleIP, vehiclePort, IPPROTO_TCP);
			if (connectionStatus == SOCKET_OK) {
				controlThread     = connection.createRecvThread(this);
				showCommandWindow = true;
			}
		} else if (connectionStatus == SOCKET_OK
		           && ImGui::Button("Disconnect")) {
			disconnect();
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
		for (const auto& module : modules) {
			module->render();
		}
	}
	ImGui::End();
}

void Dashboard::commandHandler() {
	if (showCommandWindow) {
		commandPanel();
	}
	for (const auto& module : modules) {
		if (module->shouldSendCmd()) {
			auto data = module->getCmdData();
			connection.send(data.first, data.second);
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
