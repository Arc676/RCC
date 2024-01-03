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

NetworkStream::NetworkStream(const int port, const int protocol) {
	status = initServer(port, protocol);
}

enum SocketStatus NetworkStream::initServer(const int port,
                                            const int protocol) {
	clientSock     = 0;
	this->protocol = protocol;
	this->port     = port;

	const int type = typeForProtocol(protocol);
	sock           = socket(AF_INET, type, protocol);
	if (sock < 0) {
		return CREATE_FAILED;
	}

	struct sockaddr_in myAddr;
	myAddr.sin_family      = AF_INET;
	myAddr.sin_port        = htons(port);
	myAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(sock, (struct sockaddr*)&myAddr, sizeof(myAddr)) < 0) {
		return BIND_FAILED;
	}

	struct linger opt = {1, 0};
	setsockopt(sock, SOL_SOCKET, SO_LINGER, &opt, sizeof(opt));

	if (listen(sock, 1) < 0) {
		return LISTEN_FAILED;
	}

	return SOCKET_OK;
}

enum SocketStatus NetworkStream::acceptConnection() {
	socklen_t len = sizeof(clientAddr);
	clientSock    = accept(sock, &clientAddr, &len);
	return clientSock < 0 ? ACCEPT_FAILED : SOCKET_OK;
}

NetworkStream::NetworkStream(const char* const host, const int port,
                             const int protocol) {
	status = initClient(host, port, protocol);
}

enum SocketStatus NetworkStream::initClient(const char* const host,
                                            const int port,
                                            const int protocol) {
	clientSock     = 0;
	this->protocol = protocol;
	this->port     = port;
	// NOLINTNEXTLINE (strncpy_s not in gcc)
	strncpy(ipAddress, host, IP_ADDR_BUFLEN);

	const int type = typeForProtocol(protocol);
	sock           = socket(AF_INET, type, protocol);
	if (sock < 0) {
		return CREATE_FAILED;
	}

	struct sockaddr_in hostAddr;
	hostAddr.sin_family      = AF_INET;
	hostAddr.sin_port        = htons(port);
	hostAddr.sin_addr.s_addr = inet_addr(host);
	if (connect(sock, (struct sockaddr*)&hostAddr, sizeof(hostAddr)) < 0) {
		return CONNECT_FAILED;
	}

	return SOCKET_OK;
}

void NetworkStream::disconnect() {
	shutdown(sock, SHUT_RDWR);
	close(sock);
	status = DISCONNECTED;
}

void NetworkStream::disconnectClient() {
	shutdown(clientSock, SHUT_RDWR);
	close(clientSock);
	status = DISCONNECTED;
}

size_t NetworkStream::send(const byte* const data, const size_t len) const {
	const int socket = clientSock != 0 ? clientSock : sock;

	// send message via TCP
	if (protocol == IPPROTO_TCP) {
		return write(socket, data, len);
	}

	// send message via UDP
	size_t sent      = 0;
	size_t remaining = len;
	for (const byte* ptr = data; remaining > 0;) {
		const size_t toSend = remaining > MAX_UDP_LEN ? MAX_UDP_LEN : remaining;
		sent += sendto(socket, ptr, toSend, 0, &clientAddr,
		               sizeof(struct sockaddr));
		ptr += toSend;
		remaining -= toSend;
	}
	return sent;
}

void NetworkStream::recvLoop(MessageHandler* handler) const {
	const int socket = clientSock != 0 ? clientSock : sock;
	while (!handler->shouldTerminate()) {
		const size_t bytes = read(socket, (void*)msgBuffer, MESSAGE_BUFLEN);
		handler->handleMessage(msgBuffer, bytes);
		// NOLINTNEXTLINE (memset_s not in gcc)
		memset((void*)msgBuffer, 0, MESSAGE_BUFLEN);
	}
}
