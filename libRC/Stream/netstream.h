#ifndef NETWORK_STREAM_H
#define NETWORK_STREAM_H

#include <sys/socket.h>

#include <cstddef>
#include <thread>

#include "interface.h"

#define IP_ADDR_BUFLEN 30
#define MESSAGE_BUFLEN 255

#define MAX_UDP_LEN 65507

class MessageHandler {
public:
	MessageHandler()          = default;
	virtual ~MessageHandler() = default;

	MessageHandler(const MessageHandler&)            = delete;
	MessageHandler(MessageHandler&&)                 = default;
	MessageHandler& operator=(const MessageHandler&) = delete;
	MessageHandler& operator=(MessageHandler&&)      = default;

	/**
	 * @brief Indicates that messages should no longer be received on a given
	 * stream
	 *
	 * @return Whether to terminate the message reception loop
	 */
	[[nodiscard]] virtual bool shouldTerminate() const = 0;

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
	ACCEPT_WAITING,
	ACCEPT_FAILED,
	INVALID_REQUEST,
};

/**
 * @brief Decode socket status
 *
 * @param status Socket status
 * @return Human-readable description of the socket state
 */
const char* getSocketError(enum SocketStatus status);

class NetworkStream {
	int protocol                   = 0;
	int sock                       = 0;
	char ipAddress[IP_ADDR_BUFLEN] = {0};
	int port                       = 0;

	int clientSock = 0;
	struct sockaddr clientAddr {};

	byte msgBuffer[MESSAGE_BUFLEN] = {0};

	enum SocketStatus status = DISCONNECTED;

	[[nodiscard]] size_t receive() const;

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
	 * @brief Post-construction server-side initialization
	 *
	 * @param port
	 * @param protocol
	 * @return Socket status
	 */
	enum SocketStatus initServer(int port, int protocol);

	/**
	 * @brief Post-construction client-side initialization
	 *
	 * @param host
	 * @param port
	 * @param protocol
	 * @return Socket status
	 */
	enum SocketStatus initClient(const char* host, int port, int protocol);

	/**
	 * @brief Get the socket state of the network stream
	 *
	 * @return Current socket state
	 */
	[[nodiscard]] enum SocketStatus getStatus() const { return status; }

	/**
	 * @brief Accept a new client connection
	 *
	 * @return Whether the client connection was successfully accepted
	 */
	enum SocketStatus acceptConnection();

	/**
	 * @brief Sends a message to the connected machine
	 *
	 * @param data Message buffer
	 * @param len Message length
	 * @return Number of bytes sent
	 */
	size_t send(const byte* data, size_t len) const;

	/**
	 * @brief Message receiving loop
	 *
	 * @param handler Message handler for the incoming messages
	 */
	void recvLoop(MessageHandler* handler) const;

	/**
	 * @brief Create a new thread on which to receive messages using the stream
	 *
	 * @param handler The message handler
	 * @return Thread on which the receive-loop runs
	 */
	std::thread createRecvThread(MessageHandler* handler) const;

	/**
	 * @brief Disconnect from the host (clients) or shut down the server socket
	 * (servers)
	 */
	void disconnect();

	/**
	 * @brief Disconnect the connected client (servers only)
	 */
	void disconnectClient();
};

#endif
