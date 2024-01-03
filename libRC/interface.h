#ifndef INTERFACE_H
#define INTERFACE_H

typedef unsigned char byte;

// "Open RC Vehicle" on a 10-key layout
#define DEFAULT_CONTROL_PORT 6728
// "Open RC Stream"
#define DEFAULT_RC_PORT 6727

// See OpCodes.md for bitmask table
constexpr byte OPCODE_ID_BITMASK = 0b01111000;
constexpr byte ERROR_BIT         = 0x80;
constexpr byte CORE_CMD          = 0x10;
constexpr byte MISC_CMD          = 0x20;
constexpr byte CAMERA_CMD        = 0x40;

// Core commands
constexpr byte SHUTDOWN = CORE_CMD | 0x07;

constexpr byte RC_QUERY  = CORE_CMD | 0x01;
constexpr byte RC_CONFIG = CORE_CMD | 0x04;

constexpr byte RC_OK    = RC_QUERY;
constexpr byte RC_ERROR = ERROR_BIT | RC_QUERY;

// Ping commands
constexpr byte PING     = MISC_CMD | 0x01;
constexpr byte PING_ACK = MISC_CMD | 0x02;

constexpr byte PING_INVALID = ERROR_BIT | PING;

// Camera commands
constexpr byte CAM_QUERY      = CAMERA_CMD | 0x01;
constexpr byte CAM_ACTIVATE   = CAMERA_CMD | 0x03;
constexpr byte CAM_CONFIGURE  = CAMERA_CMD | 0x04;
constexpr byte CAM_START      = CAMERA_CMD | 0x05;
constexpr byte CAM_DEACTIVATE = CAMERA_CMD | 0x07;

constexpr byte CAM_STATE = CAMERA_CMD | 0x02;
constexpr byte CAM_PROPS = CAMERA_CMD | 0x04;

constexpr byte CAM_OK    = CAMERA_CMD | 0x01;
constexpr byte CAM_ERROR = ERROR_BIT | CAMERA_CMD | 0x07;

#endif
