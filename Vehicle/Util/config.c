#include "config.h"

#include <getopt.h>
#include <stdio.h>
#include <stdlib.h>

#include "logging.h"

void defaultSetup(struct CmdlineArgs* const args) {
#define DEFAULT_CONTROL_PORT 6728;  // "Open RC Vehicle" on a 10-key layout
	args->controlPort = DEFAULT_CONTROL_PORT;
	args->verbosity   = 0;
}

void dumpSetup(const struct CmdlineArgs* const args, const unsigned verbosity) {
	logger(verbosity, DEBUG,
	       "Configured parameters:\n\tControl port: %d\n\tVerbosity: %u\n",
	       args->controlPort, args->verbosity);
}

void printHelp() {
	printf(
		"Arguments:\n\
    -p <port>, --port=<port>\tsets the port on which to listen for controllers\n\
    -v, --verbose[=level]\tsets verbosity level\n\
    --help\t\t\tshows this help message\n");
}

int parseArgs(int argc, char** argv, struct CmdlineArgs* args) {
	defaultSetup(args);

	static const char* SHORT_OPTS = "vp:";
#define LONG_COUNT 3
	static const struct option LONG_OPTS[LONG_COUNT + 1] = {
		{"help", no_argument, NULL, 0},
		{"verbose", optional_argument, NULL, 'V'},
		{"port", required_argument, NULL, 'p'},
		{0, 0, 0, 0}};

	while (1) {
		int idx = 0;
		// NOLINTNEXTLINE(concurrency-mt-unsafe)
		int opt = getopt_long(argc, argv, SHORT_OPTS, LONG_OPTS, &idx);

		// End of args
		if (opt == -1) {
			break;
		}

		switch (opt) {
			case 0:
				printHelp();
				return 1;
			case 'p':
				args->controlPort = atoi(optarg);
				break;
			case 'V':
				if (optarg) {
					args->verbosity = atoi(optarg);
					break;
				}
			case 'v':
				args->verbosity++;
				break;
		}
	}
	return 0;
}