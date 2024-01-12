#include "vehicle.h"

#include <netinet/in.h>

#include <csignal>
#include <cstddef>
#include <cstring>

#include "Modules/modules.h"
#include "Modules/ping.h"
#include "Stream/buffer.h"
#include "Stream/netstream.h"
#include "Util/config.h"
#include "Util/logging.h"
#include "interface.h"

Responder* Vehicle::getResponder(const byte opCode) {
	switch (opCode) {
		case PING:
			return &pingReply;
		case RC_QUERY:
		case RC_CONFIG:
		case RC_STOP:
			return &rc;
		default:
			if ((opCode & OPCODE_ID_BITMASK) == CAMERA_CMD) {
				return &camHandler;
			}
			return nullptr;
	}
}

void Vehicle::handleMessage(const byte* msg, size_t len) {
	static Buffer<byte> response;

	if (len == 0) {
		connected = false;
		controlStream.disconnectClient();
		return;
	}

	Buffer buf(msg, len);
	byte opCode = buf.peek();

	switch (opCode) {
		case SHUTDOWN:
			Logger::log(INFO, "Client requested shutdown.\n");
			shutdownRequested = true;
			connected         = false;
			controlStream.disconnectClient();
			break;
		default: {
			response.clear();

			Responder* responder = getResponder(opCode);
			if (responder != nullptr) {
				bool success = responder->respond(buf, response);
				if (success) {
					size_t len = response.size();
					if (len > 0) {
						Logger::log(
							DEBUG,
							"\"%s\" responder prepared %zu bytes for reply "
							"to command 0x%02X\n",
							responder->name(), len, msg[0]);
						controlStream.send(response.getBuffer(), len);
					}
				} else {
					Logger::log(ERROR,
					            "Command 0x%02X was improperly dispatched to "
					            "\"%s\" module\n",
					            opCode, responder->name());
				}
			} else {
				Logger::log(WARN, "Unhandled command: 0x%02X\n", msg[0]);
			}
			break;
		}
	}
}

// NOLINTNEXTLINE(*c-arrays)
Vehicle::Vehicle(int argc, char* argv[])
	: opts(argc, argv) {
	if (opts.helpWasRequested()) {
		return;
	}

	Logger::setVerbosity(opts.getVerbosity());

	Logger::log(INFO, "Options parsed.\n");
	opts.dump();

	Logger::log(INFO, "Starting server...\n");

	const auto res =
		controlStream.initServer(opts.getControlPort(), IPPROTO_TCP);
	if (res != SOCKET_OK) {
		Logger::log(ERROR, "Failed to start server: %s\nExiting.\n",
		            getSocketError(res));
		startupSuccessful = false;
		return;
	}
}

void Vehicle::run() {
	while (!shutdownRequested) {
		Logger::log(INFO, "Listening for client...\n");
		const enum SocketStatus res = controlStream.acceptConnection();
		if (res != SOCKET_OK) {
			Logger::log(ERROR, "%s\n", getSocketError(res));
			continue;
		}

		Logger::log(INFO, "Got connection.\n");
		connected = true;
		controlStream.recvLoop(this);

		Logger::log(INFO, "Client disconnected.\n");
	}

	Logger::log(INFO, "Shutting down...\n");
	controlStream.disconnect();
}

void Vehicle::shutdown() {
	shutdownRequested = true;
	if (connected) {
		connected = false;
		controlStream.disconnectClient();
	}
	controlStream.disconnect();
}

Vehicle* vptr = nullptr;  // NOLINT(*global-variables)

void sighandler(int /*unused*/) {
	if (vptr != nullptr) {
		vptr->shutdown();
	}
}

int main(int argc, char* argv[]) {
	Vehicle vehicle{argc, argv};
	vptr = &vehicle;

	signal(SIGINT, sighandler);

	if (vehicle) {
		vehicle.run();
	}
	return 0;
}
