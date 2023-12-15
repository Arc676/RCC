#include "logging.h"

#include <stdarg.h>
#include <stdio.h>

void logger(unsigned verbosity, unsigned level, const char* fmt, ...) {
	if (verbosity < level) {
		return;
	}
	va_list args;
	va_start(args, fmt);

	switch (level) {
		default:
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
	vprintf(fmt, args);
	va_end(args);
}