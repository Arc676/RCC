#include "Util/config.h"
#include "Util/logging.h"

int main(int argc, char* argv[]) {
	struct CmdlineArgs opts;
	if (parseArgs(argc, argv, &opts)) {
		return 0;
	}

	logger(opts.verbosity, INFO, "Options parsed.\n");
	dumpSetup(&opts, opts.verbosity);

	logger(opts.verbosity, INFO, "Starting server...\n");

	logger(opts.verbosity, INFO, "Shutting down...\n");
	return 0;
}
