#include "config.h"

#include <getopt.h>

#include <array>
#include <cstdlib>
#include <iostream>

#include "interface.h"
#include "logging.h"

void CmdlineArgs::dump() const {
	Logger::log(DEBUG,
	            "Configured parameters:\n\tControl port: %d\n\tVerbosity: %u\n",
	            controlPort, verbosity);
}

void CmdlineArgs::printHelp() {
	std::cout << "Arguments:\n\
    -p <port>, --port=<port>\tsets the port on which to listen for controllers\n\
    -v, --verbose[=level]\tsets verbosity level\n\
    --help\t\t\tshows this help message"
			  << std::endl;
}

CmdlineArgs::CmdlineArgs(int argc, char** argv) {
	static const char* SHORT_OPTS = "vp:";
	constexpr unsigned LONG_COUNT = 3;
	static const std::array<struct option, LONG_COUNT + 1> LONG_OPTS{
		{{"help", no_argument, nullptr, 0},
	     {"verbose", optional_argument, nullptr, 'V'},
	     {"port", required_argument, nullptr, 'p'},
	     {nullptr, 0, nullptr, 0}}};

	while (true) {
		int idx = 0;
		// NOLINTBEGIN(concurrency-mt-unsafe)
		const int opt =
			getopt_long(argc, argv, SHORT_OPTS, LONG_OPTS.data(), &idx);
		// NOLINTEND(concurrency-mt-unsafe)

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
					// If no verbosity level was provided, fall though to normal
					// short form flag
					break;
				}
			case 'v':
				verbosity++;
				break;
			default:
				break;
		}
	}
}
