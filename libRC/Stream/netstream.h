#ifndef NETWORK_STREAM_H
#define NETWORK_STREAM_H

#define IP_ADDR_BUFLEN 30

#ifdef __cplusplus
extern "C" {
#endif

struct NetworkStream {
	int protocol;
	int socket;
	char ipAddress[IP_ADDR_BUFLEN];
	int port;
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

#ifdef __cplusplus
}
#endif

#endif
