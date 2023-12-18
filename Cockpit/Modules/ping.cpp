#include "ping.h"

#include "imgui.h"
#include "interface.h"

void PingModule::render() {
	if (ImGui::Button("Ping Vehicle")) {
		// TODO
		response = PING_ACK;
	}
	if (response != 0x0) {
		ImGui::SameLine();
		ImGui::Text("Response: 0x%02x (%f ms)", response, latency);
	}
}
