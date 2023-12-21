#ifndef NETWORK_STREAM_H
#define NETWORK_STREAM_H

#include <stddef.h>
#include <sys/socket.h>

#include "interface.h"

#define IP_ADDR_BUFLEN 30
#define MESSAGE_BUFLEN 255

#define MAX_UDP_LEN 65507

class MessageHandler {
public:
	/**
	 * @brief Indicates that messages should no longer be received on a given
	 * stream
	 *
	 * @return Whether to terminate the message reception loop
	 */
	virtual bool shouldTerminate() const = 0;

	/**
	 * @brief Handle an incoming message from a client
	 *
	 * @param msg Incoming message
	 * @param len Message length
	 */
	virtual void handleMessage(const byte* msg, size_t len) = 0;
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

class NetworkStream {
	int protocol;
	int sock;
	char ipAddress[IP_ADDR_BUFLEN];
	int port;

	int clientSock;
	struct sockaddr clientAddr;

	byte msgBuffer[MESSAGE_BUFLEN];

	enum SocketStatus status = DISCONNECTED;

public:
	// server init
	NetworkStream(int, int);

	// client init
	NetworkStream(const char*, int, int);

	NetworkStream() = default;

	void initClient(const char*, int, int);

	enum SocketStatus getStatus() const { return status; }

	enum SocketStatus acceptConnection();

	size_t send(const byte*, size_t) const;

	void recvLoop(MessageHandler*) const;

	void disconnect() const;

	void disconnectClient() const;
};

#endif
