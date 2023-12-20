#ifndef CONFIG_H
#define CONFIG_H

#include "interface.h"

class CmdlineArgs {
	unsigned verbosity = 0;

	int controlPort = DEFAULT_CONTROL_PORT;

	bool helpRequested = false;
	static void printHelp();

public:
	void dump() const;

	CmdlineArgs(int argc, char* argv[]);

	unsigned getVerbosity() const { return verbosity; }

	int getControlPort() const { return controlPort; }

	bool helpWasRequested() const { return helpRequested; }
};

#endif
