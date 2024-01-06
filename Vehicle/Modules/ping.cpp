#include "ping.h"

#include "interface.h"

// NOLINTNEXTLINE(misc-unused-parameters)
bool Ping::respond(Buf& msg, struct Response& resp) {
	resp << PING_ACK;
	return true;
}
