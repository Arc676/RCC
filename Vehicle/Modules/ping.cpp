#include "ping.h"

#include "interface.h"

// NOLINTNEXTLINE(misc-unused-parameters)
void Ping::respond(Buf& msg, struct Response& resp) {
	resp << PING_ACK;
}
