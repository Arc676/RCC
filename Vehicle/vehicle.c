#include "config.h"
#include "logging.h"

int main(int argc, char* argv[]) {
	struct CmdlineArgs opts;
	parseArgs(argc, argv, &opts);

	logger(opts.verbosity, INFO, "Options parsed. Starting server...\n");

	logger(opts.verbosity, INFO, "Shutting down...\n");
	return 0;
}