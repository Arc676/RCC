#ifndef PING_MODULE_H
#define PING_MODULE_H

#include <stddef.h>

#include "modules.h"

void replyPing(const byte*, size_t, struct Response*);

#endif
