#include "ping.h"

#include "interface.h"

// NOLINTNEXTLINE(misc-unused-parameters)
void Ping::respond(const byte* _msg, const size_t _len, struct Response& resp) {
	resp << PING_ACK;
}
