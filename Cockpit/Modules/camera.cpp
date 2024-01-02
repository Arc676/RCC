#include "camera.h"

#include <cstddef>

#include "Peripherals/camera.h"
#include "imgui.h"
#include "interface.h"

void CameraModule::render() {
	renderController();
	if (state.cameraIsRunning()) {
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

void CameraModule::dstSelect() {
	auto& dst = state.getStreamDst();
	ImGui::RadioButton("Stream to cockpit", (int*)&dst,
	                   CameraState::STREAM_TO_CONTROLLER);
	ImGui::RadioButton("Stream to another device", (int*)&dst,
	                   CameraState::STREAM_TO_EXTERNAL);
	ImGui::RadioButton("Stream to disk on vehicle", (int*)&dst,
	                   CameraState::STREAM_TO_DISK);

	if ((dst & CameraState::STREAM_IS_NAMED) != 0) {
		ImGui::InputText("Stream destination", state.getStreamBuf(),
		                 DST_BUFLEN);
	}
	if ((dst & CameraState::STREAM_HAS_PORT) != 0) {
		ImGui::InputInt("Port", &state.getStreamPort());
	}
}

void CameraModule::cameraControls() {
	if (state.cameraIsEnabled()) {
		if (ImGui::Button("Reconfigure")) {
			setCmd(CAM_CONFIGURE);
			requestCmd();
		}
		ImGui::SameLine();
		if (state.cameraIsRunning()) {
			if (ImGui::Button("Release Camera")) {
				setCmd(CAM_DEACTIVATE);
				requestCmd();
			}
		} else if (ImGui::Button("Start Camera")) {
			setCmd(CAM_START);
			requestCmd();
		}
	} else {
		if (ImGui::Button("Acquire Camera")) {
			setCmd(CAM_ACTIVATE);
			requestCmd();
		}
	}
}

void CameraModule::renderController() {
	if (ImGui::CollapsingHeader("Camera")) {
		const auto size = state.getDeserializedSize();
		if (size == 0) {
			ImGui::Text("No camera data");
		} else if (state.getCameraNames().empty()) {
			ImGui::Text("No cameras available");
		} else {
			ImGui::Text("Camera state: %s",
			            state.cameraIsEnabled() ? "enabled" : "disabled");

			// camera configuration
			cameraSelect();
			roleSelect();
			dstSelect();

			// camera controls
			cameraControls();
		}

		if (ImGui::Button("Refresh camera information")) {
			setCmd(CAM_QUERY);
			requestCmd();
		}
		if (size > 0) {
			ImGui::Text("Received %zu bytes of camera data with last query",
			            size);
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
			if (state.cameraIsRunning()) {
				const auto& names = state.getCameraNames();
				unsigned idx      = state.getSelected();
				if (idx < names.size()) {
					viewfinderTitle = "Camera: " + names[idx];
				} else {
					viewfinderTitle = "Unknown camera";
				}
			}
			break;
		default:
			break;
	}
}
