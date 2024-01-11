#include "rc.h"

#include <SDL_error.h>
#include <SDL_events.h>
#include <SDL_gamecontroller.h>
#include <SDL_joystick.h>
#include <SDL_keyboard.h>
#include <SDL_scancode.h>
#include <SDL_timer.h>
#include <netinet/in.h>

#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <iostream>
#include <thread>
#include <utility>
#include <variant>

#include "Controls/input.h"
#include "Stream/netstream.h"
#include "Stream/rc.h"
#include "dashboard.h"
#include "imgui.h"
#include "interface.h"

void RCModule::streamSetup() {
	if (setup.running) {
		ImGui::Text("Vehicle is listening on port %d using %s", setup.port,
		            setup.protocol == IPPROTO_TCP ? "TCP" : "UDP");
	} else {
		ImGui::RadioButton("TCP", &setup.protocol, IPPROTO_TCP);
		ImGui::SameLine();
		ImGui::RadioButton("UDP", &setup.protocol, IPPROTO_UDP);
		ImGui::InputInt("RC Port", &setup.port);
	}
	if (ImGui::Button("Query Status")) {
		requestCmd(RC_QUERY);
	}

	ImGui::SameLine();
	if (remoteSockState == SOCKET_OK) {
		if (ImGui::Button("Disable Stream")) {
			stopTransmitting();
			requestCmd(RC_STOP);
		}
	} else {
		if (ImGui::Button("Configure Stream")) {
			static byte buf[1 + sizeof(RCSetup)] = {RC_CONFIG};
			memcpy(buf + 1, &setup, sizeof(RCSetup));
			requestCmd(buf, sizeof(buf));
		}
	}
}

void RCModule::render() {
	if (ImGui::CollapsingHeader("Remote Control Stream")) {
		streamSetup();

		transmissionControls();
		renderSocketStates();

		if (!lastReadOK) {
			ImGui::Text(
				"Failed to read expected data from last response from vehicle");
		}

		if (showControlSetup) {
			if (ImGui::Begin("Vehicle Controls", &showControlSetup)) {
				changeControls();
			}
			ImGui::End();
		} else if (ImGui::Button("Change Controller Inputs")) {
			showControlSetup = true;
		}
	}
}

void RCModule::changeControls() {
	if (ImGui::BeginTabBar("ControllerTabs")) {
		if (ImGui::BeginTabItem("Keyboard")) {
			showControls(true);
			ImGui::EndTabItem();
		}
		if (ImGui::BeginTabItem("Joystick")) {
			showControls(false);
			ImGui::EndTabItem();
		}
		ImGui::EndTabBar();
	}
}

const char* RCModule::joystickName() const {
	if (selectedJoystick < 0) {
		return "Please select...";
	}
	return SDL_GameControllerNameForIndex(selectedJoystick);
}

void RCModule::joystickSelect() {
	const int stickCount = SDL_NumJoysticks();
	if (stickCount == 0) {
		ImGui::Text("No joysticks detected");
	} else if (ImGui::BeginCombo("Select Joystick", joystickName())) {
		for (int i = 0; i < stickCount; i++) {
			const bool selected = i == selectedJoystick;
			if (ImGui::Selectable(SDL_GameControllerNameForIndex(i),
			                      selected)) {
				if (joystick != nullptr) {
					SDL_GameControllerClose(joystick);
				}
				joystick = SDL_GameControllerOpen(i);
				if (joystick == nullptr) {
					joystickError = SDL_GetError();
				} else {
					selectedJoystick = i;
					joystickID       = SDL_JoystickInstanceID(
                        SDL_GameControllerGetJoystick(joystick));
					joystickError = nullptr;
				}
				if (selected) {
					ImGui::SetItemDefaultFocus();
				}
			}
		}
		ImGui::EndCombo();
	}
	if (joystickError != nullptr) {
		ImGui::Text("Failed to enable joystick: %s", joystickError);
	}
}

void RCModule::showControls(const bool keyboard) {
	if (!keyboard) {
		joystickSelect();
	}
	if (ImGui::BeginTable("ControlTable", 2)) {
		ImGui::TableSetupColumn("Control");
		if (keyboard) {
			ImGui::TableSetupColumn("Key");
		} else {
			ImGui::TableSetupColumn("Axis/Button");
		}
		ImGui::TableHeadersRow();

		for (auto it = ism.begin(); it != ism.end(); it++) {
			const auto& [handler, inputs] = *it;
			const ControlID& input = keyboard ? inputs.first : inputs.second;
			if (!isValidMapping(input)) {
				continue;
			}

			ImGui::TableNextColumn();
			ImGui::Text("%s", handler.getName());
			ImGui::TableNextColumn();
			const char* inputName;
			if (listener.active && listener.it == it) {
				inputName = "Waiting...";
			} else {
				inputName = getInputName(input);
			}
			if (ImGui::Selectable(inputName)) {
				listener = {true, keyboard, it};
			}
		}

		ImGui::EndTable();
	}
	if (inputsChanged && ImGui::Button("Save Changes")) {
		controls      = createInputMap(ism);
		inputsChanged = false;
	}
	if (duplicateInput.exists()) {
		ImGui::Text("Failed to set input: cannot set %s for both %s and %s",
		            duplicateInput.name, duplicateInput.ctrl1,
		            duplicateInput.ctrl2);
	}
}

void RCModule::startTransmitting() {
	const char* address = getDashboard()->getDeviceAddress();
	assert(address != nullptr);
	localSockState = stream.initClient(address, setup.port, setup.protocol);
	if (localSockState == SOCKET_OK) {
		transmitThread = std::thread(&RCModule::transmitLoop, this);
	}
}

void RCModule::stopTransmitting() {
	if (localSockState != SOCKET_OK) {
		return;
	}
	auto tmp        = remoteSockState;
	remoteSockState = DISCONNECTED;
	stream.disconnect();
	transmitThread.join();
	remoteSockState = tmp;
	localSockState  = DISCONNECTED;
}

void RCModule::transmissionControls() {
	if (remoteSockState == SOCKET_OK) {
		ImGui::SameLine();
		if (transmitThread.joinable()) {
			if (ImGui::Button("Stop Transmitting")) {
				stopTransmitting();
			}
		} else {
			if (ImGui::Button("Start Transmitting")) {
				startTransmitting();
			}
		}
	}

	setTransmissionDelay();
}

void RCModule::setTransmissionDelay() {
	ImGui::SliderInt("Milliseconds between transmissions", &transmitDelay, -1,
	                 MAX_TRANSMISSION_DELAY);
	if (transmitDelay <= 0) {
		ImGui::Text("Cockpit will transmit controls as frequently as possible");
	}
}

void RCModule::renderSocketStates() const {
	ImGui::Text("RC stream transmission: %s", getSocketError(localSockState));

	switch (remoteSockState) {
		case DISCONNECTED:
			ImGui::Text("Control stream not running on vehicle");
			break;
		case SOCKET_OK:
			if (transmitThread.joinable()) {
				ImGui::Text("Transmitting controls...");
			} else {
				ImGui::Text("Vehicle control stream is ready to receive");
			}
			break;
		default:
			ImGui::Text("Failed to set up RC stream: %s",
			            getSocketError(remoteSockState));
			break;
	}
}

void RCModule::handleMessage(ConstBuf& msg) {
	byte response;
	msg >> response;
	lastReadOK = true;
	if (response == RC_OK) {
		const auto* last = checkLastCmd().first;
		switch (last[0]) {
			case RC_CONFIG: {
				remoteSockState = SOCKET_OK;
				startTransmitting();
				break;
			}
			case RC_QUERY:
				msg >> setup >> remoteSockState;
				lastReadOK = msg.ok();
				break;
			case RC_STOP:
				msg >> remoteSockState;
				lastReadOK = msg.ok();
				break;
		}
	} else {
		msg >> remoteSockState;
	}
}

bool RCModule::isDuplicateInput(bool keyboard, const ControlID& cid) {
	for (const auto& [handler, inputs] : ism) {
		if (cid == (keyboard ? inputs.first : inputs.second)) {
			duplicateInput = {getInputName(cid), listener.it->first.getName(),
			                  handler.getName()};
			return true;
		}
	}
	return false;
}

bool RCModule::interceptInput(const SDL_Event* const event) {
	if (event->type == SDL_KEYDOWN
	    && event->key.keysym.scancode == SDL_SCANCODE_ESCAPE) {
		return true;
	}
	ControlID newInput = SDL_SCANCODE_UNKNOWN;
	if (listener.keyboard) {
		if (event->type != SDL_KEYDOWN) {
			return false;
		}
		newInput = event->key.keysym.scancode;
	} else {
		switch (event->type) {
			case SDL_CONTROLLERAXISMOTION:
				newInput = (SDL_GameControllerAxis)event->caxis.axis;
				break;
			case SDL_CONTROLLERBUTTONDOWN:
				newInput = (SDL_GameControllerButton)event->cbutton.button;
				break;
			default:
				return false;
		}
	}
	auto& toReplace = listener.keyboard ? listener.it->second.first
	                                    : listener.it->second.second;
	if (newInput != toReplace) {
		if (isDuplicateInput(listener.keyboard, newInput)) {
			return true;
		}
		toReplace     = newInput;
		inputsChanged = true;
		duplicateInput.clear();
		return true;
	}
	return false;
}

void RCModule::handleEvent(const SDL_Event* const event) {
	if (listener.active && interceptInput(event)) {
		listener.active = false;
		return;
	}
	float btnState = 1;
	ControlID id;
	switch (event->type) {
		// gamepad buttons
		case SDL_CONTROLLERBUTTONUP:
			btnState = 0;
		case SDL_CONTROLLERBUTTONDOWN:
			id = (SDL_GameControllerButton)event->cbutton.button;
			break;
		// keyboard input
		case SDL_KEYUP:
			btnState = 0;
		case SDL_KEYDOWN:
			id = event->key.keysym.scancode;
			break;
		// gamepad joysticks
		case SDL_CONTROLLERAXISMOTION:
			if (event->caxis.which != joystickID) {
				return;
			}
			id = (SDL_GameControllerAxis)event->caxis.axis;
			btnState =
				(RCState::CC_t)event->caxis.value / INT16_MAX * RCState::CC_MAX;
			break;
		// joystick disconnection
		case SDL_CONTROLLERDEVICEREMOVED:
			if (event->jdevice.which == joystickID) {
				SDL_GameControllerClose(joystick);
				joystick         = nullptr;
				joystickError    = "The selected joystick was disconnected";
				selectedJoystick = joystickID = -1;
			}
			break;
	}
	const auto& control = controls.find(id);
	if (control != controls.end()) {
		control->second(btnState);
	}
}

void RCModule::transmitLoop() const {
	while (remoteSockState == SOCKET_OK) {
		stream.send((byte*)&state, sizeof(RCState));
		if (transmitDelay > 0) {
			SDL_Delay(transmitDelay);
		}
	}
}
