#include "ping.h"

#include "imgui.h"
#include "messages.h"

void PingModule::render() {
	if (ImGui::Button("Ping Vehicle")) {
		// TODO
		response = PING_ACK;
	}
	if (response != 0x0) {
		ImGui::SameLine();
		ImGui::Text("Response: %#03x (%f ms)", response, latency);
	}
}
