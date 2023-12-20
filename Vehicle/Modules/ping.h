#ifndef PING_MODULE_H
#define PING_MODULE_H

#include <stddef.h>

#include "modules.h"

#ifdef __cplusplus
extern "C" {
#endif

void replyPing(const byte*, size_t, struct Response*);

#ifdef __cplusplus
}
#endif

#endif
