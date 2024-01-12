#ifndef CONFIG_H
#define CONFIG_H

#include "interface.h"

class CmdlineArgs {
	unsigned verbosity = 0;

	int controlPort = DEFAULT_CONTROL_PORT;

	bool helpRequested = false;

	/**
	 * @brief Prints a help message to stdout
	 */
	static void printHelp();

public:
	/**
	 * @brief Dumps the configuration to stdout
	 */
	void dump() const;

	/**
	 * @brief Parse the command line arguments
	 *
	 * @param argc Argument count
	 * @param argv Argument vector
	 */
	CmdlineArgs(int argc, char* argv[]);  // NOLINT(*c-arrays)

	[[nodiscard]] unsigned getVerbosity() const { return verbosity; }

	[[nodiscard]] int getControlPort() const { return controlPort; }

	[[nodiscard]] bool helpWasRequested() const { return helpRequested; }
};

#endif
