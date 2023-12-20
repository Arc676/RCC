#include "ping.h"

#include <cstddef>
#include <ctime>

#include "imgui.h"
#include "interface.h"

void PingModule::render() {
	if (ImGui::Button("Ping Vehicle")) {
		latency = time(nullptr);
		requestCmd();
	}
	if (response != 0x0) {
		ImGui::SameLine();
		ImGui::Text("Response: 0x%02x (%ld ms)", response, latency);
	}
}

const char* PingModule::decodeResponse() const {
	switch (response) {
		case PING_ACK:
			return "Ping acknowledged";
		case PING_INVALID:
			return "Invalid ping response";
		default:
			return "Unknown response";
	}
}

void PingModule::handleMessage(const char* msg, size_t len) {
	response = msg[0];
	latency  = time(nullptr) - latency;
	if (len != 1) {
		response = PING_INVALID;
		return;
	}
}
