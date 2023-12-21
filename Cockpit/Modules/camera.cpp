#include "camera.h"

#include <cstddef>

#include "imgui.h"
#include "interface.h"

void CameraModule::render() {
	renderController();
	if (state.enabled) {
		renderViewfinder();
	}
}

void CameraModule::renderController() {
	if (ImGui::CollapsingHeader("Camera")) {
		if (state.cameras.empty()) {
			ImGui::Text("No cameras available");
		} else {
			if (ImGui::BeginCombo("Selected camera",
			                      state.cameras[state.selectedCam].c_str())) {
				for (int i = 0; i < state.cameras.size(); i++) {
					const bool selected = state.selectedCam == i;
					if (ImGui::Selectable(state.cameras[i].c_str(), selected)) {
						state.selectedCam = i;
						viewfinderTitle   = "Camera: " + state.cameras[i];
					}

					if (selected) {
						ImGui::SetItemDefaultFocus();
					}
				}
				ImGui::EndCombo();
			}
		}

		ImGui::Text("Camera state: %s", state.enabled ? "enabled" : "disabled");
		if (ImGui::Button("Refresh camera information")) {
			setCmd(CAM_QUERY);
			requestCmd();
		}
	}
}

void CameraModule::renderViewfinder() {
	if (ImGui::Begin(viewfinderTitle.c_str())) {
	}
	ImGui::End();
}

void CameraModule::handleMessage(const byte* const msg, const size_t len) {
	switch (msg[0]) {
		case CAM_STATE:
			state.deserialize(msg + 1, len - 1);
			break;
		default:
			break;
	}
}
