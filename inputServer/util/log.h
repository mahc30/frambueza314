// log.h

#ifndef LOG_H
#define LOG_H

#include <stdio.h>
#include <stdarg.h>
#include <time.h>

// Log levels
typedef enum {
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR
} log_level_t;

// Function declarations
void Log_msg(const char *format, ...);

// Optional: level-specific wrappers
#define log_debug(fmt, ...) Log_msg(LOG_DEBUG, fmt, ##__VA_ARGS__)
#define log_info(fmt, ...)  Log_msg(LOG_INFO,  fmt, ##__VA_ARGS__)
#define log_warn(fmt, ...)  Log_msg(LOG_WARN,  fmt, ##__VA_ARGS__)
#define log_error(fmt, ...) Log_msg(LOG_ERROR, fmt, ##__VA_ARGS__)

#endif // LOG_H
