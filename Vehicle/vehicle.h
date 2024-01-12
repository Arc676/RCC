#ifndef VEHICLE_H
#define VEHICLE_H

#include <array>
#include <cstddef>

#include "Modules/camera.h"
#include "Modules/modules.h"
#include "Modules/ping.h"
#include "Modules/rc.h"
#include "Stream/netstream.h"
#include "Util/config.h"

class Vehicle : public MessageHandler {
	CmdlineArgs opts;
	NetworkStream controlStream;
	bool connected         = false;
	bool shutdownRequested = false;

	bool startupSuccessful = true;

	Ping pingReply;
	Camera camHandler;
	RC rc;

protected:
	/**
	 * @brief Get the responder for a given command
	 *
	 * @param opCode Command byte
	 * @return Pointer to the appropriate responder
	 */
	Responder* getResponder(byte opCode);

public:
	/**
	 * @brief Construct a new Vehicle from command line options
	 *
	 * @param argc Number of command line arguments
	 * @param argv Command line arguments
	 */
	Vehicle(int argc, char* argv[]);  // NOLINT(*c-arrays)

	/**
	 * @brief Vehicle server loop
	 */
	void run();

	void handleMessage(const byte* msg, size_t len) override;

	[[nodiscard]] bool shouldTerminate() const override { return !connected; };

	void shutdown();

	explicit operator bool() const { return startupSuccessful; }
};

#endif
