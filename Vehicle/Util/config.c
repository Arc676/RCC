#include "config.h"

#include <getopt.h>
#include <stdlib.h>

void parseArgs(int argc, char** argv, struct CmdlineArgs* args) {
	args->verbosity = 0;

#define LONG_COUNT 1
	static const char* SHORT_OPTS = "v";

	static const struct option LONG_OPTS[LONG_COUNT + 1] = {
		{"verbose", optional_argument, NULL, 'V'}, {0, 0, 0, 0}};

	while (1) {
		int idx = 0;
		// NOLINTNEXTLINE(concurrency-mt-unsafe)
		int opt = getopt_long(argc, argv, SHORT_OPTS, LONG_OPTS, &idx);

		if (opt == -1) {
			break;
		}

		switch (opt) {
			case 'V':
				args->verbosity = atoi(optarg);
			case 'v':
				args->verbosity++;
				break;
		}
	}
}