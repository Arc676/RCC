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

RC::~RC() {
	if (rcThread.joinable()) {
		stopStream();
	}
}

void RC::stopStream() {
	if (setup.protocol == IPPROTO_TCP) {
		stopAccepting = true;
		stream.disconnectClient();
	}
	stopReceiving = true;
	stream.disconnect();
	rcThread.join();
}

enum SocketStatus RC::setupStream() {
	// disconnect existing stream to reconfigure
	if (stream.getStatus() == SOCKET_OK) {
		stopStream();
	}
	const auto res = stream.initServer(setup.port, setup.protocol);
	if (res == SOCKET_OK) {
		if (setup.protocol == IPPROTO_TCP) {
			Logger::log(DEBUG, "Setting up read loop for TCP RC stream\n");
			stopAccepting = false;
			rcThread      = std::thread([this]() {
                while (!stopAccepting) {
                    Logger::log(DEBUG, "Waiting for RC stream to connect...\n");
                    auto res = stream.acceptConnection();
                    if (res == SOCKET_OK) {
                        Logger::log(
                            DEBUG,
                            "RC stream connected. Starting read loop...\n");
                        stopReceiving = false;
                        stream.recvLoop(this);
                        Logger::log(DEBUG, "RC stream disconnected\n");
                    } else {
                        Logger::log(DEBUG, "%s\n", getSocketError(res));
                        stream.disconnect();
                        break;
                    }
                }
            });
		} else {
			Logger::log(DEBUG, "Setting up read loop for UDP RC stream\n");
			stopReceiving = false;
			rcThread      = stream.createRecvThread(this);
		}
	}
	return res;
}

void RC::respond(const byte* const msg, const size_t len,
                 struct Response& response) {
	switch (msg[0]) {
		case RC_QUERY:
			response << RC_OK << setup << stream.getStatus();
			break;
		case RC_CONFIG:
			if (len >= 1 + sizeof(RCSetup)) {
				memcpy(&setup, msg + 1, sizeof(RCSetup));
				const auto res = setupStream();
				if (res == SOCKET_OK) {
					response << RC_OK;
				} else {
					response << RC_ERROR << res;
				}
			} else {
				response << RC_ERROR << INVALID_REQUEST;
			}
			break;
		case RC_STOP:
			stopStream();
			response << RC_OK << stream.getStatus();
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
		stopReceiving = true;
		return;
	}
	if (len < sizeof(RCState)) {
		return;
	}
	memcpy(&state, msg, sizeof(RCState));
	printf("Vehicle got %.02f %.02f %.02f\r", state.acceleration, state.brakes,
	       state.steering);
}
