#ifndef RC_STREAM_H
#define RC_STREAM_H

#include <netinet/in.h>

#include "interface.h"

struct RCSetup {
	bool running = false;
	int port     = DEFAULT_RC_PORT;
	int protocol = IPPROTO_UDP;
};

#endif
