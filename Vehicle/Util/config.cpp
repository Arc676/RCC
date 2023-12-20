#include "config.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "interface.h"
#include "logging.h"

void CmdlineArgs::dump() const {
	Logger::log(DEBUG,
	            "Configured parameters:\n\tControl port: %d\n\tVerbosity: %u\n",
	            controlPort, verbosity);
}

void CmdlineArgs::printHelp() {
	printf(
		"Arguments:\n\
    -p <port>, --port=<port>\tsets the port on which to listen for controllers\n\
    -v, --verbose[=level]\tsets verbosity level\n\
    --help\t\t\tshows this help message\n");
}

CmdlineArgs::CmdlineArgs(int argc, char** argv) {
	static const char* SHORT_OPTS = "vp:";
#define LONG_COUNT 3
	static const struct option LONG_OPTS[LONG_COUNT + 1] = {
		{"help", no_argument, NULL, 0},
		{"verbose", optional_argument, NULL, 'V'},
		{"port", required_argument, NULL, 'p'},
		{0, 0, 0, 0}};

	while (true) {
		int idx = 0;
		// NOLINTNEXTLINE(concurrency-mt-unsafe)
		const int opt = getopt_long(argc, argv, SHORT_OPTS, LONG_OPTS, &idx);

		// End of args
		if (opt == -1) {
			break;
		}

		switch (opt) {
			case 0:
				printHelp();
				helpRequested = true;
				break;
			case 'p':
				controlPort = atoi(optarg);
				break;
			case 'V':
				if (optarg != nullptr) {
					verbosity = atoi(optarg);
					break;
				}
			case 'v':
				verbosity++;
				break;
		}
	}
}
