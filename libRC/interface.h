#ifndef INTERFACE_H
#define INTERFACE_H

typedef unsigned char byte;

#define DEFAULT_CONTROL_PORT 6728  // "Open RC Vehicle" on a 10-key layout

// See OpCodes.md for bitmask table
#define ERROR_CORE_FLAG 0x80
#define CAMERA_FLAG     0x40

// Core commands
#define SHUTDOWN 0x8D

// Ping commands
#define PING         0x01
#define PING_ACK     0x02
#define PING_INVALID 0x81

// Camera commands
#define CAM_QUERY      0x41
#define CAM_ACTIVATE   0x4A
#define CAM_DEACTIVATE 0x4D

#endif
