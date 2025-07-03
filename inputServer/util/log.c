// log.c

#include "log.h"

void Log_msg(const char *format, ...) {
    const char *level_str[] = {"DEBUG", "INFO", "WARN", "ERROR"};

    // Get current time
    time_t now = time(NULL);
    struct tm *t = localtime(&now);

    char time_buf[20];
    strftime(time_buf, sizeof(time_buf), "%Y-%m-%d %H:%M:%S", t);

    // Print prefix
    fprintf(stderr, "[%s] [%s] ", time_buf, level_str[1]);

    // Print formatted message
    va_list args;
    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);

    fprintf(stderr, "\n");
}
