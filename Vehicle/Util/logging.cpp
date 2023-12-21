#include "logging.h"

#include <stdarg.h>
#include <stdio.h>

unsigned Logger::verbosity = 0;

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

	va_list args;
	va_start(args, fmt);
	vprintf(fmt, args);
	va_end(args);
}
