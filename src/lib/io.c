#include <stdarg.h>
#include "string.h"
#include "mini_uart.h"

int printf(const char *format, ...) {
    va_list arg;
    int done = 0;
    va_start (arg, format);
    char buffer[1024];
    memset(buffer, 0, sizeof(char) * 20);
    const char *ptr = format;
    while(*ptr) {
        if (*ptr == '%') {
            switch (*(++ptr)) {
                case 's':
                    puts(va_arg(arg, char*));
                    break;
                case 'd':
                    itoa(va_arg(arg, int), buffer, 10);
                    puts(buffer);
                    memset(buffer, 0, sizeof(char) * 20);
                    break;
                case 'b':
                    itoa(va_arg(arg, int), buffer, 2);
                    puts(buffer);
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