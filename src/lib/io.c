#include <stdarg.h>
#include "string.h"
#include "mini_uart.h"

int printf(const char *format, ...) {
    va_list arg;
    int done = 0;
    va_start (arg, format);
    char buffer[20];
    memset(buffer, 0, sizeof(char) * 20);
    const char *ptr = format;
    while(*ptr != '\0') {
        if (*ptr == '%') {
            switch (*(++ptr)) {
                case 's':
                    puts(va_arg(arg, char*));
                    break;
                case 'd':
                    puts(itoa(va_arg(arg, int), buffer, 10));
                    memset(buffer, 0, sizeof(char) * 20);
                    break;
                default:
                    break;
            }
        } else {
            putchar(*ptr);
        }

        ptr++;
    }

    va_end(arg);
    return done;
}