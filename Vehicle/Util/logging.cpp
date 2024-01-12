#include "logging.h"

#include <cstdarg>
#include <cstdio>

unsigned Logger::verbosity = 0;  // NOLINT(*global-variables)

void Logger::log(unsigned level, const char* fmt, ...) {
	if (verbosity < level) {
		return;
	}

	switch (level) {
		default:
		case DEBUG:
			printf("[DEBUG] ");
			break;
		case INFO:
			printf("[INFO] ");
			break;
		case WARN:
			printf("[WARN] ");
			break;
		case ERROR:
			printf("[ERR] ");
			break;
	}

	// NOLINTBEGIN(*decay)
	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
	// NOLINTEND(*decay)
}
