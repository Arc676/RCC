#include "vehicle.h"

#include <netinet/in.h>
#include <stddef.h>
#include <string.h>

#include "Modules/modules.h"
#include "Modules/ping.h"
#include "Stream/netstream.h"
#include "Util/config.h"
#include "Util/logging.h"
#include "interface.h"

Vehicle* Vehicle::instance = nullptr;

bool connected = false;
int isDisconnected() {
	return !connected;
}

Responder getResponder(const byte cmd) {
	switch (cmd) {
		case PING:
			return replyPing;
		default:
			return NULL;
	}
}

void Vehicle::handler(const byte* msg, size_t len) {
	static struct Response response;

	if (len == 0) {
		connected = false;
		netstream_disconnectClient(&controlStream);
		return;
	}

	// NOLINTNEXTLINE (memset_s not in gcc)
	memset(&response, 0, sizeof(struct Response));
	switch (msg[0]) {
		case SHUTDOWN:
			Logger::log(INFO, "Client requested shutdown.\n");
			shutdownRequested = true;
			connected         = false;
			netstream_disconnectClient(&controlStream);
			break;
		default: {
			Responder responder = getResponder(msg[0]);
			if (responder) {
				responder(msg, len, &response);

				if (response.len > 0) {
					netstream_send(&controlStream, response.data, response.len);
				}
			} else {
				Logger::log(WARN, "Unhandled command: 0x%02X\n", msg[0]);
			}
		}
	}
}

Vehicle::Vehicle(int argc, char* argv[])
	: opts(argc, argv) {
	if (opts.helpWasRequested()) {
		return;
	}

	Vehicle::instance = this;
	Logger::setVerbosity(opts.getVerbosity());

	Logger::log(INFO, "Options parsed.\n");
	opts.dump();

	Logger::log(INFO, "Starting server...\n");

	enum SocketStatus res = netstream_initServer(
		&controlStream, opts.getControlPort(), IPPROTO_TCP);
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
		enum SocketStatus res = netstream_acceptConnection(&controlStream);
		if (res != SOCKET_OK) {
			Logger::log(ERROR, "%s\n", getSocketError(res));
		}

		Logger::log(INFO, "Got connection.\n");
		connected = true;
		netstream_recvLoop(&controlStream, Vehicle::handle, isDisconnected);

		Logger::log(INFO, "Client disconnected.\n");
	}

	Logger::log(INFO, "Shutting down...\n");
	netstream_disconnect(&controlStream);
}

int main(int argc, char* argv[]) {
	Vehicle vehicle{argc, argv};
	if (vehicle) {
		vehicle.run();
	}
	return 0;
}
