#include "ping.h"

#include "interface.h"

// NOLINTNEXTLINE(misc-unused-parameters)
void replyPing(const byte* msg, size_t _len, struct Response* const resp) {
	resp->len     = 1;
	resp->data[0] = PING_ACK;
}
