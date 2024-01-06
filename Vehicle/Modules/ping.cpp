#include "ping.h"

#include "interface.h"

// NOLINTNEXTLINE(misc-unused-parameters)
bool Ping::respond(ConstBuf& msg, Buf& resp) {
	resp << PING_ACK;
	return true;
}
