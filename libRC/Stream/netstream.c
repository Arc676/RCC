#include "netstream.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>

const char* getSocketError(const enum SocketStatus status) {
	switch (status) {
		case DISCONNECTED:
			return "Socket disconnected";
		case SOCKET_OK:
			return "Socket OK";
		case CREATE_FAILED:
			return "Failed to create socket";
		case BIND_FAILED:
			return "Failed to bind socket";
		case LISTEN_FAILED:
			return "Failed to listen on socket";
		case CONNECT_FAILED:
			return "Failed to connect to host";
		default:
			return "Unknown socket error";
	}
}

int typeForProtocol(const int protocol) {
	return protocol == IPPROTO_TCP ? SOCK_STREAM : SOCK_DGRAM;
}

enum SocketStatus netstream_initServer(struct NetworkStream* const stream,
                                       const int port, const int protocol) {
	stream->protocol = protocol;
	stream->port     = port;

	const int type = typeForProtocol(protocol);
	stream->socket = socket(AF_INET, type, protocol);
	if (stream->socket < 0) {
		return CREATE_FAILED;
	}

	struct sockaddr_in myAddr;
	myAddr.sin_family      = AF_INET;
	myAddr.sin_port        = htons(port);
	myAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(stream->socket, (struct sockaddr*)&myAddr, sizeof(myAddr)) < 0) {
		return BIND_FAILED;
	}

	struct linger opt = {1, 0};
	setsockopt(stream->socket, SOL_SOCKET, SO_LINGER, &opt, sizeof(opt));

	if (listen(stream->socket, 1) < 0) {
		return LISTEN_FAILED;
	}

	return SOCKET_OK;
}

enum SocketStatus netstream_initClient(struct NetworkStream* const stream,
                                       const char* const host, const int port,
                                       const int protocol) {
	stream->protocol = protocol;
	stream->port     = port;
	// NOLINTNEXTLINE (strncpy_s not in gcc)
	strncpy(stream->ipAddress, host, IP_ADDR_BUFLEN);

	const int type = typeForProtocol(protocol);
	stream->socket = socket(AF_INET, type, protocol);
	if (stream->socket < 0) {
		return CREATE_FAILED;
	}

	struct sockaddr_in hostAddr;
	hostAddr.sin_family      = AF_INET;
	hostAddr.sin_port        = htons(port);
	hostAddr.sin_addr.s_addr = inet_addr(host);
	if (connect(stream->socket, (struct sockaddr*)&hostAddr, sizeof(hostAddr))
	    < 0) {
		return CONNECT_FAILED;
	}

	return SOCKET_OK;
}
