#include "rc.h"

#include <SDL_events.h>
#include <SDL_scancode.h>
#include <SDL_timer.h>
#include <netinet/in.h>

#include <cassert>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <thread>

#include "Stream/netstream.h"
#include "Stream/rc.h"
#include "dashboard.h"
#include "imgui.h"
#include "interface.h"

void RCModule::render() {
	if (ImGui::CollapsingHeader("Remote Control Stream")) {
		if (setup.running) {
			ImGui::Text("Vehicle is listening on port %d using %s", setup.port,
			            setup.protocol == IPPROTO_TCP ? "TCP" : "UDP");
		} else {
			ImGui::RadioButton("TCP", &setup.protocol, IPPROTO_TCP);
			ImGui::SameLine();
			ImGui::RadioButton("UDP", &setup.protocol, IPPROTO_UDP);
			ImGui::InputInt("RC Port", &setup.port);
		}
		if (ImGui::Button("Query Status")) {
			requestCmd(RC_QUERY);
		}

		ImGui::SameLine();
		if (remoteSockState == SOCKET_OK) {
			if (ImGui::Button("Disable Stream")) {
				stopTransmitting();
				requestCmd(RC_STOP);
			}
		} else {
			if (ImGui::Button("Configure Stream")) {
				static byte buf[1 + sizeof(RCSetup)] = {RC_CONFIG};
				memcpy(buf + 1, &setup, sizeof(RCSetup));
				requestCmd(buf, sizeof(buf));
			}
		}

		transmissionControls();
		renderSocketStates();
	}
}

void RCModule::startTransmitting() {
	const char* address = getDashboard()->getDeviceAddress();
	assert(address != nullptr);
	localSockState = stream.initClient(address, setup.port, setup.protocol);
	if (localSockState == SOCKET_OK) {
		transmitThread = std::thread(&RCModule::transmitLoop, this);
	}
}

void RCModule::stopTransmitting() {
	if (localSockState != SOCKET_OK) {
		return;
	}
	auto tmp        = remoteSockState;
	remoteSockState = DISCONNECTED;
	stream.disconnect();
	transmitThread.join();
	remoteSockState = tmp;
}

void RCModule::transmissionControls() {
	if (remoteSockState == SOCKET_OK) {
		ImGui::SameLine();
		if (transmitThread.joinable()) {
			if (ImGui::Button("Stop Transmitting")) {
				stopTransmitting();
			}
		} else {
			if (ImGui::Button("Start Transmitting")) {
				startTransmitting();
			}
		}
	}

	setTransmissionDelay();
}

void RCModule::setTransmissionDelay() {
	ImGui::SliderInt("Milliseconds between transmissions", &transmitDelay, -1,
	                 MAX_TRANSMISSION_DELAY);
	if (transmitDelay <= 0) {
		ImGui::Text("Cockpit will transmit controls as frequently as possible");
	}
}

void RCModule::renderSocketStates() const {
	ImGui::Text("Control stream transmission: %s",
	            getSocketError(localSockState));

	switch (remoteSockState) {
		case DISCONNECTED:
			ImGui::Text("Control stream not running on vehicle");
			break;
		case SOCKET_OK:
			if (transmitThread.joinable()) {
				ImGui::Text("Transmitting controls...");
			} else {
				ImGui::Text("Vehicle control stream is ready to receive");
			}
			break;
		default:
			ImGui::Text("Failed to set up RC stream: %s",
			            getSocketError(remoteSockState));
			break;
	}
}

void RCModule::handleMessage(const byte* const buf, size_t len) {
	if (buf[0] == RC_OK) {
		const auto* last = checkLastCmd().first;
		switch (last[0]) {
			case RC_CONFIG: {
				remoteSockState = SOCKET_OK;
				startTransmitting();
				break;
			}
			case RC_QUERY:
				if (len >= 1 + sizeof(RCSetup) + sizeof(enum SocketStatus)) {
					memcpy(&setup, buf + 1, sizeof(RCSetup));
					memcpy(&remoteSockState, buf + 1 + sizeof(RCSetup),
					       sizeof(enum SocketStatus));
				}
				break;
			case RC_STOP:
				if (len >= 1 + sizeof(enum SocketStatus)) {
					memcpy(&remoteSockState, buf + 1,
					       sizeof(enum SocketStatus));
				}
				break;
		}
	} else {
		memcpy(&remoteSockState, buf + 1, sizeof(enum SocketStatus));
	}
}

void RCModule::handleEvent(const SDL_Event* const event) {
	float btnState = 1;
	switch (event->type) {
		case SDL_KEYUP:
			btnState = 0;
		case SDL_KEYDOWN: {
			const auto& keys    = controls[KEYBOARD];
			const auto& control = keys.find(event->key.keysym.scancode);
			if (control != keys.end()) {
				control->second(btnState);
			}
		} break;
	}
}

void RCModule::transmitLoop() const {
	while (remoteSockState == SOCKET_OK) {
		printf("Cockpit sent %.02f %.02f %.02f\r", state.acceleration,
		       state.brakes, state.steering);
		stream.send((byte*)&state, sizeof(RCState));
		if (transmitDelay > 0) {
			SDL_Delay(transmitDelay);
		}
	}
}
