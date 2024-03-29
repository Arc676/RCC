#include "netstream.h"

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <algorithm>
#include <cstddef>
#include <cstring>
#include <thread>

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
		case ACCEPT_WAITING:
			return "Waiting for client";
		case ACCEPT_FAILED:
			return "Failed to accept connection";
		case INVALID_REQUEST:
			return "Invalid request";
		default:
			return "Unknown socket error";
	}
}

int typeForProtocol(const int protocol) {
	return protocol == IPPROTO_TCP ? SOCK_STREAM : SOCK_DGRAM;
}

NetworkStream::NetworkStream(const int port, const int protocol)
	: status(initServer(port, protocol)) {}

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

	struct sockaddr_in myAddr {};
	myAddr.sin_family      = AF_INET;
	myAddr.sin_port        = htons(port);
	myAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	// NOLINTNEXTLINE(*cstyle-cast)
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
	status        = ACCEPT_WAITING;
	socklen_t len = sizeof(clientAddr);
	clientSock    = accept(sock, &clientAddr, &len);
	status        = clientSock < 0 ? ACCEPT_FAILED : SOCKET_OK;
	return status;
}

NetworkStream::NetworkStream(const char* const host, const int port,
                             const int protocol)
	: status(initClient(host, port, protocol)) {}

enum SocketStatus NetworkStream::initClient(const char* const host,
                                            const int port,
                                            const int protocol) {
	clientSock     = 0;
	this->protocol = protocol;
	this->port     = port;

	const int type = typeForProtocol(protocol);
	sock           = socket(AF_INET, type, protocol);
	if (sock < 0) {
		return CREATE_FAILED;
	}

	struct sockaddr_in hostAddr {};
	hostAddr.sin_family      = AF_INET;
	hostAddr.sin_port        = htons(port);
	hostAddr.sin_addr.s_addr = inet_addr(host);
	// NOLINTNEXTLINE(*cstyle-cast)
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
		// NOLINTNEXTLINE(*pointer-arithmetic)
		ptr += toSend;
		remaining -= toSend;
	}
	return sent;
}

size_t NetworkStream::receive() {
	if (protocol == IPPROTO_TCP) {
		const int socket = clientSock != 0 ? clientSock : sock;
		return read(socket, msgBuffer.data(), MESSAGE_BUFLEN);
	}
	struct sockaddr addr {};
	socklen_t len = sizeof(sockaddr);
	return recvfrom(sock, msgBuffer.data(), MESSAGE_BUFLEN, 0, &addr, &len);
}

void NetworkStream::recvLoop(MessageHandler* handler) {
	while (!handler->shouldTerminate()) {
		const size_t bytes = receive();
		handler->handleMessage(msgBuffer.data(), bytes);
		std::ranges::fill(msgBuffer, (byte)0);
	}
}

std::thread NetworkStream::createRecvThread(MessageHandler* const handler) {
	return std::thread(&NetworkStream::recvLoop, this, handler);
}
