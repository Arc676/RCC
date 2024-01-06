#include "ping.h"

#include <chrono>
#include <cstddef>
#include <ctime>

#include "imgui.h"
#include "interface.h"

void PingModule::render() {
	if (ImGui::CollapsingHeader("Ping")) {
		if (ImGui::Button("Ping Vehicle")) {
			auto now = std::chrono::system_clock::now();
			latency  = std::chrono::duration_cast<std::chrono::microseconds>(
                now.time_since_epoch());
			response = 0;
			requestCmd();
		}
		if (response != 0x0) {
			ImGui::Text("Response: 0x%02X (%s)", response, decodeResponse());
			ImGui::Text("Latency: %ld us", latency.count());
		} else if (latency > std::chrono::microseconds(0)) {
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

void PingModule::handleMessage(ConstBuf& msg) {
	byte response;
	msg >> response;

	auto now = std::chrono::system_clock::now();
	latency  = std::chrono::duration_cast<std::chrono::microseconds>(
                  now.time_since_epoch())
	          - latency;
	if (msg.size() != 1) {
		response = PING_INVALID;
		return;
	}
}
