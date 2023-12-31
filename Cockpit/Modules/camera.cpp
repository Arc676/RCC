#include "camera.h"

#include <cstddef>

#include "imgui.h"
#include "interface.h"

void CameraModule::render() {
	renderController();
	if (state.cameraIsEnabled()) {
		renderViewfinder();
	}
}

void CameraModule::cameraSelect() {
	const auto& cameras = state.getCameraNames();
	if (ImGui::BeginCombo("Selected camera",
	                      cameras[state.getSelected()].c_str())) {
		for (int i = 0; i < cameras.size(); i++) {
			const bool selected = state.getSelected() == i;
			if (ImGui::Selectable(cameras[i].c_str(), selected)) {
				state.selectCamera(i);
				viewfinderTitle = "Camera: " + cameras[i];
			}

			if (selected) {
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
}

void CameraModule::roleSelect() {
	ImGui::Checkbox("Raw", &state.getRoles().raw);
	ImGui::SameLine();
	ImGui::Checkbox("Stills", &state.getRoles().stills);
	ImGui::SameLine();
	ImGui::Checkbox("Video", &state.getRoles().video);
	ImGui::SameLine();
	ImGui::Checkbox("Viewfinder", &state.getRoles().viewfinder);
}

void CameraModule::renderController() {
	if (ImGui::CollapsingHeader("Camera")) {
		if (state.getCameraNames().empty()) {
			ImGui::Text("No cameras available");
		} else {
			cameraSelect();
			roleSelect();
		}

		ImGui::Text("Camera state: %s",
		            state.cameraIsEnabled() ? "enabled" : "disabled");
		if (ImGui::Button("Refresh camera information")) {
			setCmd(CAM_QUERY);
			requestCmd();
		}
		const auto size = state.getDeserializedSize();
		if (size > 0) {
			ImGui::Text("Received %zu data bytes from last query", size);
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
