#ifndef LOGGING_H
#define LOGGING_H

// Logging severity levels
#define DEBUG 4
#define INFO  3
#define WARN  2
#define ERROR 1

class Logger {
	static unsigned verbosity;

public:
	static void setVerbosity(unsigned level) { verbosity = level; }

	static void log(unsigned, const char* fmt, ...);
};

#endif
