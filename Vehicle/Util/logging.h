#ifndef LOGGING_H
#define LOGGING_H

// Logging severity levels
#define DEBUG 4
#define INFO  3
#define WARN  2
#define ERROR 1

#ifdef __cplusplus
extern "C" {
#endif

void logger(unsigned verbosity, unsigned level, const char* fmt, ...);

#ifdef __cplusplus
}
#endif

#endif
