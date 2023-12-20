#include "netstream.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <stddef.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

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
		case ACCEPT_FAILED:
			return "Failed to accept connection";
		default:
			return "Unknown socket error";
	}
}

int typeForProtocol(const int protocol) {
	return protocol == IPPROTO_TCP ? SOCK_STREAM : SOCK_DGRAM;
}

enum SocketStatus netstream_initServer(struct NetworkStream* const stream,
                                       const int port, const int protocol) {
	stream->clientSocket = 0;
	stream->protocol     = protocol;
	stream->port         = port;

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

enum SocketStatus netstream_acceptConnection(
	struct NetworkStream* const stream) {
	socklen_t len        = sizeof(stream->clientAddr);
	stream->clientSocket = accept(stream->socket, &stream->clientAddr, &len);
	return stream->clientSocket < 0 ? ACCEPT_FAILED : SOCKET_OK;
}

enum SocketStatus netstream_initClient(struct NetworkStream* const stream,
                                       const char* const host, const int port,
                                       const int protocol) {
	stream->clientSocket = 0;
	stream->protocol     = protocol;
	stream->port         = port;
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

void netstream_disconnect(struct NetworkStream* const stream) {
	close(stream->socket);
}

size_t netstream_send(struct NetworkStream* const stream,
                      const byte* const data, const size_t len) {
	int socket = stream->clientSocket ? stream->clientSocket : stream->socket;

	// send message via TCP
	if (stream->protocol == IPPROTO_TCP) {
		return write(socket, data, len);
	}

	// send message via UDP
	size_t sent      = 0;
	size_t remaining = len;
	for (const byte* ptr = data; remaining;) {
		size_t toSend = remaining > MAX_UDP_LEN ? MAX_UDP_LEN : remaining;
		sent += sendto(socket, ptr, toSend, 0, &stream->clientAddr,
		               sizeof(struct sockaddr));
		ptr += toSend;
		remaining -= toSend;
	}
	return sent;
}

void netstream_recvLoop(struct NetworkStream* const stream,
                        const MessageHandler handler,
                        const TerminationFlag shouldTerminate) {
	while (!shouldTerminate()) {
		size_t bytes = read(stream->socket, stream->msgBuffer, MESSAGE_BUFLEN);
		handler(stream->msgBuffer, bytes);
		// NOLINTNEXTLINE (memset_s not in gcc)
		memset(stream->msgBuffer, 0, MESSAGE_BUFLEN);
	}
}
