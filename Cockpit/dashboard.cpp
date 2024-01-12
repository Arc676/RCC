#include "dashboard.h"

#include <netinet/in.h>

#include <cstddef>
#include <cstdio>
#include <thread>

#include "Stream/buffer.h"
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
			if (connectionStatus == SOCKET_OK) {
				disconnect();
			}
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
	Buffer buf(msg, len);
	for (const auto& module : modules) {
		if (module->canHandleMessage(buf.peek())) {
			module->handleMessage(buf);
		}
	}
}

void Dashboard::connectionWindow() {
	if (ImGui::Begin("Connection", &showConnectionWindow)) {
		ImGui::InputText("Vehicle IP Address", vehicleIP.data(),
		                 IP_ADDR_BUFLEN);
		ImGui::InputInt("Control Post", &vehiclePort);
		ImGui::Text("Status: %s", getSocketError(connectionStatus));

		if (connectionStatus == DISCONNECTED && ImGui::Button("Connect")) {
			connectionStatus = connection.initClient(vehicleIP.data(),
			                                         vehiclePort, IPPROTO_TCP);
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
			const auto& data = module->getCmdData();
			connection.send(data.getBuffer(), data.size());
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
