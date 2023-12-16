#ifndef CONFIG_H
#define CONFIG_H

struct CmdlineArgs {
	unsigned verbosity;

	int controlPort;
};

void parseArgs(int argc, char* argv[], struct CmdlineArgs* args);

#endif