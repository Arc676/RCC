#ifndef INTERFACE_H
#define INTERFACE_H

typedef unsigned char byte;

#define DEFAULT_CONTROL_PORT 6728  // "Open RC Vehicle" on a 10-key layout

#define PING         0x01
#define PING_ACK     0x02
#define PING_INVALID 0xF1

#define SHUTDOWN 0xBD

#endif
