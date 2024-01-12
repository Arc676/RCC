#ifndef VEHICLE_MODULES_H
#define VEHICLE_MODULES_H

#include <concepts>
#include <cstddef>
#include <cstring>
#include <string>
#include <type_traits>

#include "Stream/buffer.h"
#include "Stream/netstream.h"
#include "interface.h"

class Responder {
protected:
	using ConstBuf = Buffer<const byte>;
	using Buf      = Buffer<byte>;

public:
	Responder()                            = default;
	virtual ~Responder()                   = default;
	Responder(const Responder&)            = delete;
	Responder(Responder&&)                 = default;
	Responder& operator=(const Responder&) = delete;
	Responder& operator=(Responder&&)      = default;

	/**
	 * @brief Respond to an incoming command
	 *
	 * @param msg Command buffer
	 * @param response Response object in which to construct reply message, if
	 * any
	 * @return Whether the command was successfully handled
	 */
	virtual bool respond(ConstBuf& msg, Buf& response) = 0;

	[[nodiscard]] virtual const char* name() const = 0;
};

#endif
