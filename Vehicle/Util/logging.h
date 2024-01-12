#ifndef LOGGING_H
#define LOGGING_H

// Logging severity levels
#define DEBUG 4
#define INFO  3
#define WARN  2
#define ERROR 1

class Logger {
	static unsigned verbosity;  // NOLINT(*global-variables)

public:
	static void setVerbosity(unsigned level) { verbosity = level; }

	/**
	 * @brief Print the given message to console if the verbosity level is
	 * sufficient
	 *
	 * @param level The requisite verbosity level for the message to be printed
	 * @param fmt Format string for the message to print
	 * @param ... Additional arguments for the format string
	 */
	static void log(unsigned level, const char* fmt, ...);
};

#endif
