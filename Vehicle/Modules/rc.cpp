#include "rc.h"

#include <netinet/in.h>

#include <cstddef>
#include <cstdio>
#include <cstring>
#include <thread>

#include "Drivers/interface.h"
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
	if (rcThread.joinable()) {
		rcThread.join();
	}
}

enum SocketStatus RC::setupStream() {
	// disconnect existing stream to reconfigure
	if (rcThread.joinable()) {
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

bool RC::respond(ConstBuf& msg, Buf& response) {
	byte opCode = 0;
	msg >> opCode;
	switch (opCode) {
		case RC_QUERY:
			response << RC_OK << setup << stream.getStatus();
			return true;
		case RC_CONFIG:
			msg >> setup;
			if (msg.ok()) {
				const auto res = setupStream();
				if (res == SOCKET_OK) {
					response << RC_OK;
				} else {
					response << RC_ERROR << res;
				}
			} else {
				response << RC_ERROR << INVALID_REQUEST;
			}
			return true;
		case RC_STOP:
			stopStream();
			response << RC_OK << stream.getStatus();
			return true;
		default:
			return false;
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
	driver->update(state);
}
