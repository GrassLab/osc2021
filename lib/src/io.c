#include <stdarg.h>
#include "string.h"
#include "mini_uart.h"
#define BUFFERSIZE (1024)

int printf(const char *format, ...) {
    va_list arg;
    int done = 0;
    va_start (arg, format);
    char buffer[BUFFERSIZE];
    memset(buffer, 0, sizeof(char) * BUFFERSIZE);
    const char *ptr = format;

    int long_flag = 0;
    while(*ptr) {
        if (*ptr == '%' || long_flag) {
            switch (*(++ptr)) {
                case 's':
                    puts(va_arg(arg, char*));
                    break;
                case 'd':
                    itoa(va_arg(arg, int), buffer, 10);
                    puts(buffer);
                    memset(buffer, 0, sizeof(char) * BUFFERSIZE);
                    long_flag = 0;
                    break;
                case 'b':
                    itoa(va_arg(arg, int), buffer, 2);
                    puts(buffer);
                    memset(buffer, 0, sizeof(char) * BUFFERSIZE);
                    long_flag = 0;
                    break;
                case 'l':
                    long_flag = 1;
                    ptr--;
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
