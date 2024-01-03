#include "rc.h"

#include <netinet/in.h>

#include <cstddef>
#include <cstring>

#include "Stream/netstream.h"
#include "imgui.h"
#include "interface.h"

void RCModule::render() {
	if (ImGui::CollapsingHeader("Remote Control Stream")) {
		if (setup.running) {
			ImGui::Text("Vehicle is listening on port %d using %s", setup.port,
			            setup.proto == IPPROTO_TCP ? "TCP" : "UDP");
		} else {
			ImGui::RadioButton("TCP", &setup.proto, IPPROTO_TCP);
			ImGui::SameLine();
			ImGui::RadioButton("UDP", &setup.proto, IPPROTO_UDP);
			ImGui::InputInt("RC Port", &setup.port);
		}
		if (ImGui::Button("Query Status")) {
			requestCmd(RC_QUERY);
		}
		if (result > SOCKET_OK) {
			ImGui::Text("Failed to set up RC stream: %s",
			            getSocketError(result));
		}
	}
}

void RCModule::handleMessage(const byte* const buf, size_t len) {
	if (buf[0] == RC_OK) {
	} else {
		memcpy(&result, buf + 1, sizeof(enum SocketStatus));
	}
}
