#include <netinet/in.h>
#include <stddef.h>
#include <string.h>

#include "Modules/modules.h"
#include "Modules/ping.h"
#include "Stream/netstream.h"
#include "Util/config.h"
#include "Util/logging.h"
#include "interface.h"

struct CmdlineArgs opts;
struct NetworkStream controlStream;
int connected         = 0;
int shutdownRequested = 0;

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

void handler(const byte* msg, size_t len) {
	static struct Response response;

	if (len == 0) {
		connected = 0;
		return;
	}

	// NOLINTNEXTLINE (memset_s not in gcc)
	memset(&response, 0, sizeof(struct Response));
	switch (msg[0]) {
		default: {
			Responder responder = getResponder(msg[0]);
			if (responder) {
				responder(msg, len, &response);

				if (response.len > 0) {
					netstream_send(&controlStream, response.data, response.len);
				}
			} else {
				logger(opts.verbosity, WARN, "Unhandled command: 0x%02X\n",
				       msg[0]);
			}
		}
	}
}

int main(int argc, char* argv[]) {
	if (parseArgs(argc, argv, &opts)) {
		return 0;
	}

	logger(opts.verbosity, INFO, "Options parsed.\n");
	dumpSetup(&opts, opts.verbosity);

	logger(opts.verbosity, INFO, "Starting server...\n");

	enum SocketStatus res =
		netstream_initServer(&controlStream, opts.controlPort, IPPROTO_TCP);
	if (res != SOCKET_OK) {
		logger(opts.verbosity, ERROR, "Failed to start server: %s\nExiting.\n",
		       getSocketError(res));
		return 1;
	}

	while (!shutdownRequested) {
		logger(opts.verbosity, INFO, "Listening for client...\n");
		res = netstream_acceptConnection(&controlStream);
		if (res != SOCKET_OK) {
			logger(opts.verbosity, ERROR, "%s\n", getSocketError(res));
			return 1;
		}

		logger(opts.verbosity, INFO, "Got connection.\n");
		connected = 1;
		netstream_recvLoop(&controlStream, handler, isDisconnected);

		logger(opts.verbosity, INFO, "Client disconnected.\n");
	}

	logger(opts.verbosity, INFO, "Shutting down...\n");
	return 0;
}
