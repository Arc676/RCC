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

/**
 * @brief Decode socket status
 *
 * @param status Socket status
 * @return Human-readable description of the socket state
 */
const char* getSocketError(enum SocketStatus status);

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
	/**
	 * @brief Initialize a server-side network stream
	 *
	 * @param port Port on which to listen for clients
	 * @param protocol Socket protocol
	 */
	NetworkStream(int port, int protocol);

	/**
	 * @brief Initialize a client-side network stream
	 *
	 * @param host Host to which to connect
	 * @param port Port on which to connect
	 * @param protocol Socket protocol
	 */
	NetworkStream(const char* host, int port, int protocol);

	NetworkStream() = default;

	/**
	 * @brief Post-construction client-side initialization
	 */
	void initClient(const char*, int, int);

	/**
	 * @brief Get the socket state of the network stream
	 *
	 * @return Current socket state
	 */
	enum SocketStatus getStatus() const { return status; }

	/**
	 * @brief Accept a new client connection
	 *
	 * @return Whether the client connection was successfully accepted
	 */
	enum SocketStatus acceptConnection();

	/**
	 * @brief Sends a message to the connected machine
	 *
	 * @param msg Message buffer
	 * @param len Message length
	 * @return Number of bytes sent
	 */
	size_t send(const byte* msg, size_t len) const;

	/**
	 * @brief Message receiving loop
	 *
	 * @param handler Message handler for the incoming messages
	 */
	void recvLoop(MessageHandler* handler) const;

	/**
	 * @brief Disconnect from the host (clients) or shut down the server socket
	 * (servers)
	 */
	void disconnect() const;

	/**
	 * @brief Disconnect the connected client (servers only)
	 */
	void disconnectClient() const;
};

#endif
