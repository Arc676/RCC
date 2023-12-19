#ifndef NETWORK_STREAM_H
#define NETWORK_STREAM_H

#include <stddef.h>
#define IP_ADDR_BUFLEN 30
#define MESSAGE_BUFLEN 255

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*MessageHandler)(const char*, size_t);

struct NetworkStream {
	int protocol;
	int socket;
	char ipAddress[IP_ADDR_BUFLEN];
	int port;

	char msgBuffer[MESSAGE_BUFLEN];
};

enum SocketStatus {
	DISCONNECTED = -1,
	SOCKET_OK    = 0,
	CREATE_FAILED,
	BIND_FAILED,
	LISTEN_FAILED,
	CONNECT_FAILED,
};

const char* getSocketError(enum SocketStatus);

enum SocketStatus netstream_initServer(struct NetworkStream*, int, int);

enum SocketStatus netstream_initClient(struct NetworkStream*, const char*, int,
                                       int);

void netstream_recvLoop(struct NetworkStream*, MessageHandler, const int*);

void netstream_disconnect(struct NetworkStream*);

#ifdef __cplusplus
}
#endif

#endif
