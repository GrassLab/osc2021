#include "printf.h"

static char printf_buffer[PRINTF_BUF_SIZE];
static size_t printf_buf_len;
static size_t printf_nwrite;

static uint32_t printf_append_C(char c){
    printf_buffer[printf_buf_len++] = c;
    if(printf_buf_len == PRINTF_BUF_SIZE - 1){
        printf_buffer[printf_buf_len] = '\0';
        printf_nwrite += uart_write(printf_buffer, printf_buf_len);
        printf_buf_len = 0;
    }
    return 1;
}

static uint32_t printf_append_S(const char *s){
    uint32_t n = 0;
    while(*s){
        printf_append_C(*s++);
        n++;
    }
    return n;
}

static uint32_t printf_number(uint64_t num, uint8_t base){
    uint32_t len = 1;
    if(num >= base){
        len += printf_number(num / base, base);
    }
    uint64_t rem = num % base;
    printf_append_C(((rem > 9)? (rem - 10) + 'a' : rem + '0'));
    return len;
}

static uint32_t printf_d(const char *fmt, va_list args, uint32_t len){
    int32_t num = va_arg(args, int32_t);
    if(num < 0){
        len += printf_append_C('-');
        num *= -1;
    }
    len += printf_number(num, 10);
    return vprintf(fmt, args, len);
}

static uint32_t printf_ld(const char *fmt, va_list args, uint32_t len){
    int64_t num = va_arg(args, int64_t);
    if(num < 0){
        len += printf_append_C('-');
        num *= -1;
    }
    len += printf_number(num, 10);
    return vprintf(fmt, args, len);
}

static uint32_t printf_u(const char *fmt, va_list args, uint32_t len){
    uint32_t num = va_arg(args, uint32_t);
    len += printf_number(num, 10);
    return vprintf(fmt, args, len);
}

static uint32_t printf_lu(const char *fmt, va_list args, uint32_t len){
    uint64_t num = va_arg(args, uint64_t);
    len += printf_number(num, 10);
    return vprintf(fmt, args, len);
}

static uint32_t printf_x(const char *fmt, va_list args, uint32_t len){
    uint32_t num = va_arg(args, uint32_t);
    len += printf_append_S("0x");
    len += printf_number(num, 16);
    return vprintf(fmt, args, len);
}

static uint32_t printf_lx(const char *fmt, va_list args, uint32_t len){
    uint64_t num = va_arg(args, uint64_t);
    len += printf_append_S("0x");
    len += printf_number(num, 16);
    return vprintf(fmt, args, len);
}

static uint32_t printf_c(const char *fmt, va_list args, uint32_t len){
    int32_t c = va_arg(args, int32_t);
    len += printf_append_C(c);
    return vprintf(fmt, args, len);
}

static uint32_t printf_s(const char *fmt, va_list args, uint32_t len){
    const char *str = va_arg(args, const char *);
    len += printf_append_S(str);
    return vprintf(fmt, args, len);
}

static uint32_t vprintf(const char *fmt, va_list args, uint32_t len){
    char c, cc;
    while(*fmt){
        c = *fmt++;
        if(c != '%'){
            len += printf_append_C(c);
        }else{
            c = *fmt++;
            switch(c){
            case 'd':
                return printf_d(fmt, args, len);
            case 'u':
                return printf_u(fmt, args, len);
            case 'x':
                return printf_x(fmt, args, len);
            case 'c':
                return printf_c(fmt, args, len);
            case 's':
                return printf_s(fmt, args, len);
            case 'p':
                return printf_lx(fmt, args, len);
            case 'l':
                cc = *fmt++;
                if(cc == 'l'){
                    cc = *fmt++;
                }
                switch(cc){
                case 'd':
                    return printf_ld(fmt, args, len);
                case 'u':
                    return printf_lu(fmt, args, len);
                case 'x':
                    return printf_lx(fmt, args, len);
                default:
                    va_arg(args, int32_t);
                    len += printf_append_C('%');
                    len += printf_append_C(c);
                    len += printf_append_C(cc);
                    continue;
                }
            default:
                va_arg(args, int32_t);
                len += printf_append_C('%');
                len += printf_append_C(c);
                break;
            }
        }
    }
    return len;
}

size_t printf(const char *fmt, ...){
    printf_buf_len = 0;
    printf_nwrite = 0;

    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args, 0);
    va_end(args);

    printf_buffer[printf_buf_len] = '\0';
    printf_nwrite += uart_write(printf_buffer, printf_buf_len);

    return printf_nwrite;
}

size_t puts(const char *str){
    printf_buf_len = 0;
    printf_append_S(str);
    printf_append_S("\r\n");
    printf_buffer[printf_buf_len] = '\0';
    return uart_write(printf_buffer, printf_buf_len);
}
