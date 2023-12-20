#ifndef NETWORK_STREAM_H
#define NETWORK_STREAM_H

#include <stddef.h>
#include <sys/socket.h>

#define IP_ADDR_BUFLEN 30
#define MESSAGE_BUFLEN 255

#define MAX_UDP_LEN 65507

#ifdef __cplusplus
extern "C" {
#endif

typedef void (*MessageHandler)(const char*, size_t);
typedef int (*TerminationFlag)();

struct NetworkStream {
	int protocol;
	int socket;
	char ipAddress[IP_ADDR_BUFLEN];
	int port;

	int clientSocket;
	struct sockaddr clientAddr;

	char msgBuffer[MESSAGE_BUFLEN];
};

enum SocketStatus {
	DISCONNECTED = -1,
	SOCKET_OK    = 0,
	CREATE_FAILED,
	BIND_FAILED,
	LISTEN_FAILED,
	CONNECT_FAILED,
	ACCEPT_FAILED,
};

const char* getSocketError(enum SocketStatus);

enum SocketStatus netstream_initServer(struct NetworkStream*, int, int);

enum SocketStatus netstream_acceptConnection(struct NetworkStream*);

enum SocketStatus netstream_initClient(struct NetworkStream*, const char*, int,
                                       int);

size_t netstream_send(struct NetworkStream*, const char*, size_t);

void netstream_recvLoop(struct NetworkStream*, MessageHandler, TerminationFlag);

void netstream_disconnect(struct NetworkStream*);

#ifdef __cplusplus
}
#endif

#endif
