#include "args.h"
#include "string.h"
#include "uart.h"
#include "data_type.h"
#include "utility.h"
#include "time.h"

#define print_buffer_size 0x100
long find_special_token (char *b) {
    for (long i = 0; b[i]; i++) {
        if (b[i] == '\n') {
            return i;
        }

        if (b[i] == '%') {
            if (b[i + 1] == 'd')
                return i;
            else if (b[i + 1] == 'f')
                return i;
            else if (b[i + 1] == 'u')
                return i;
            else if (b[i + 1] == 'x')
                return i;
            else if (b[i + 1] == 's')
                return i;
            else if (b[i + 1] == 'c')
                return i;
            else if (b[i + 1] == 'p')
                return i;
        }
    }
    return -1;
}

void kprintf (char *format, ...) {
    char buffer[print_buffer_size];
    u64 counter = 0;

    for (char *ptr = format; *ptr; ptr++) {
        if (*ptr == '%') {
            if (ptr[1] == 'd')
                counter++;
            else if (ptr[1] == 'f')
                counter++;
            else if (ptr[1] == 'u')
                counter++;
            else if (ptr[1] == 'x')
                counter++;
            else if (ptr[1] == 's')
                counter++;
            else if (ptr[1] == 'c')
                counter++;
            else if (ptr[1] == 'p')
                counter++;
        }
    }

   u64 size = strlen(format);
   va_list ap;
   va_start(ap, format);
   for (u64 i = 0; i < size;) {
        long s_size = 0;
        s_size = find_special_token(&format[i]);

        if (!s_size) {
            counter--;
            if (format[i] == '\n') {
                uart_send("\r\n");
                i += 1;
            }
            else if (format[i] == '%') {
                if (format[i + 1] == 'd') {
                    long tmp = va_arg(ap, long);
                    uart_sendi(tmp);
                }
                else if (format[i + 1] == 'f') {
                    double tmp = va_arg(ap, double);
                    uart_sendf(tmp);
                }
                else if (format[i + 1] == 'u') {
                    u64 tmp = va_arg(ap, u64);
                    uart_sendi(tmp);
                }
                else if (format[i + 1] == 'x') {
                    u64 tmp = va_arg(ap, u64);
                    uart_sendh(tmp);
                }
                else if (format[i + 1] == 's') {
                    char *tmp = va_arg(ap, char *);
                    uart_send(tmp);
                }
                else if (format[i + 1] == 'c') {
                    char tmp = va_arg(ap, int);
                    uart_sendc(tmp);
                }
                else if (format[i + 1] == 'p') {
                    uart_send("[d]");
                }
                i += 2;
            }
            else {
                uart_send("Error: print format error\r\n");
            }
        }
        else if (s_size < print_buffer_size && s_size >= 0) {
            strncopy(buffer, &format[i], s_size + 1);
            uart_send(buffer);
            i += s_size;
        }
        else {
            strncopy(buffer, &format[i], print_buffer_size);
            uart_send(buffer);
            i += print_buffer_size - 1;
        }
   }
   va_end(ap);
}

void print_el1_reg () {
}

void kprint_time () {
    kprintf("[%f] time interrupt.\n", get_time());
}
