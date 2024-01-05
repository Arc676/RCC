#include "rc.h"

#include <netinet/in.h>

#include <cstddef>
#include <cstdio>
#include <cstring>
#include <thread>

#include "Modules/modules.h"
#include "Stream/netstream.h"
#include "Stream/rc.h"
#include "Util/logging.h"
#include "interface.h"

void RC::respond(const byte* const msg, const size_t len,
                 struct Response& response) {
	switch (msg[0]) {
		case RC_QUERY:
			response << RC_OK << setup;
			break;
		case RC_CONFIG:
			if (len >= 1 + sizeof(RCSetup)) {
				// disconnect existing stream to reconfigure
				if (stream.getStatus() == SOCKET_OK) {
					stopStream = true;
					if (setup.protocol == IPPROTO_TCP) {
						stream.disconnectClient();
					}
					stream.disconnect();
					rcThread.join();
				}
				memcpy(&setup, msg + 1, sizeof(RCSetup));
				const auto res = stream.initServer(setup.port, setup.protocol);
				if (res == SOCKET_OK) {
					if (setup.protocol == IPPROTO_TCP) {
						acceptThread = std::thread([this]() {
							Logger::log(
								DEBUG,
								"Waiting for control stream to connect...\n");
							stream.acceptConnection();
							Logger::log(DEBUG, "Control stream connected\n");
							rcThread = stream.createRecvThread(this);
						});
					} else {
						rcThread = stream.createRecvThread(this);
					}
					response << RC_OK;
				} else {
					response << RC_ERROR << res;
				}
			} else {
				response << RC_ERROR << INVALID_REQUEST;
			}
			break;
		case RC_STOP:
			stopStream = true;
			break;
		default:
			Logger::log(
				ERROR,
				"Command 0x%02X was improperly dispatched to RC module\n",
				msg[0]);
			break;
	}
}

void RC::handleMessage(const byte* const msg, const size_t len) {
	if (setup.protocol == IPPROTO_TCP && len == 0) {
		stopStream = true;
		return;
	}
	memcpy(&state, msg, sizeof(RCState));
	printf("Vehicle got %.02f %.02f %.02f\r", state.acceleration, state.brakes,
	       state.steering);
}
