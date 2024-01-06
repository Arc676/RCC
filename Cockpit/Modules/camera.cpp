#include "camera.h"

#include <cstddef>
#include <cstring>

#include "Peripherals/camera.h"
#include "imgui.h"
#include "interface.h"
#include "libcamera/property_ids.h"

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
	if (ImGui::TreeNode("Camera Properties")) {
		if (ImGui::BeginTable("CamProps", 3)) {
			ImGui::TableSetupColumn("ID");
			ImGui::TableSetupColumn("Property");
			ImGui::TableSetupColumn("Value");
			ImGui::TableHeadersRow();
			for (const auto& [id, value] : camProps) {
				ImGui::TableNextColumn();
				ImGui::Text("%d", id);
				ImGui::TableNextColumn();
				const auto& it = libcamera::properties::properties.find(id);
				ImGui::Text("%s", it == libcamera::properties::properties.end()
				                      ? "Unknown property"
				                      : it->second->name().c_str());
				ImGui::TableNextColumn();
				ImGui::Text("%s", value.c_str());
			}
			ImGui::EndTable();
		}
		if (ImGui::Button("Get camera data")) {
			byte cmd[1 + sizeof(size_t)] = {CAM_QUERY};
			size_t selected              = state.getSelected();
			memcpy(cmd + 1, &selected, sizeof(size_t));
			requestCmd(cmd, sizeof(cmd));
		}
		ImGui::TreePop();
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
			requestCmd(CAM_CONFIGURE);
		}
		ImGui::SameLine();
		if (state.cameraIsRunning()) {
			if (ImGui::Button("Release Camera")) {
				requestCmd(CAM_DEACTIVATE);
			}
		} else if (ImGui::Button("Start Camera")) {
			requestCmd(CAM_START);
		}
	} else {
		if (ImGui::Button("Acquire Camera")) {
			requestCmd(CAM_ACTIVATE);
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
			requestCmd(CAM_QUERY);
		}
		if (size > 0) {
			ImGui::Text("Received %zu bytes of camera data with last query",
			            size);
		}
		if (lastResult != CameraState::CAMERA_OK) {
			ImGui::Text("Error from last operation: %s",
			            CameraState::decodeResult(lastResult));
		}
	}
}

void CameraModule::renderViewfinder() {
	if (ImGui::Begin(viewfinderTitle.c_str())) {
	}
	ImGui::End();
}

void CameraModule::handleMessage(ConstBuf& msg) {
	byte response;
	msg >> response;
	switch (response) {
		case CAM_STATE:
			msg >> state;
			if (state.cameraIsRunning()) {
				const auto& names = state.getCameraNames();
				size_t idx        = state.getSelected();
				if (idx < names.size()) {
					viewfinderTitle = "Camera: " + names[idx];
				} else {
					viewfinderTitle = "Unknown camera";
				}
			}
			break;
		case CAM_PROPS:
			msg >> camProps;
			break;
		case CAM_OK:
			requestCmd(CAM_QUERY);
			lastResult = CameraState::CAMERA_OK;
			break;
		case CAM_ERROR:
			msg >> lastResult;
			if (!msg.ok()) {
				lastResult = CameraState::UNKNOWN_ERROR;
			}
			break;
		default:
			break;
	}
}
