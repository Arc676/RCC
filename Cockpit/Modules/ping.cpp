#include "ping.h"

#include <cstddef>
#include <ctime>

#include "imgui.h"
#include "interface.h"

void PingModule::render() {
	if (ImGui::CollapsingHeader("Ping")) {
		if (ImGui::Button("Ping Vehicle")) {
			latency  = time(nullptr);
			response = 0;
			requestCmd();
		}
		if (response != 0x0) {
			ImGui::Text("Response: 0x%02x (%s)", response, decodeResponse());
			ImGui::Text("Latency: %ld ms", latency);
		} else if (latency > 0) {
			ImGui::Text("Waiting for response from vehicle...");
		}
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
