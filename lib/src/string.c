#include "string.h"
#include "args.h"
#include "io.h"
#include "uart.h"

void memset (char *dst, char n, unsigned int size) {
    for (unsigned int i = 0; i < size; i++)
        dst[i] = n;
}


void mem_set (char *dst, char value, unsigned long size) {
    unsigned long addr = (unsigned long) dst;
    unsigned long aliged_addr = (addr + 0xf) & 0xfffffffffffffff0;
    unsigned long tmp = size < aliged_addr - aliged_addr ? size : aliged_addr - aliged_addr;
    for (unsigned long i = 0; i < tmp; i++)
        dst[i] = 0;

    if (tmp == size)
        return;

    tmp = size - tmp;
    tmp = (tmp + 0xf) & 0xfffffffffffffff0;

    for (unsigned long i = 0; i < tmp / 8; i++) {
        unsigned long *ptr = (unsigned long *)aliged_addr;
        ptr[i] = 0;
    }
}

void strip_newline (char *buffer) {
    int len = strlen(buffer);
    if (len > 1 && buffer[len - 1] == '\n' && buffer[len - 2] == '\r') {
        buffer[len - 1] = '\0';
        buffer[len - 2] = '\0';
    }
    else if (len > 0 && buffer[len - 1] == '\n') {
        buffer[len - 1] = '\0';
    }
}

int strlen (const char *str) {
    int num = 0;
    while (str[num]) num++;
    return num;
}

int strcmp (const char *str1, const char *str2) {
    for (; *str1 && *str2; str1++, str2++) {
        if (*str1 > *str2)
            return -1;
        else if (*str1 < *str2)
            return 1;
    }
    if (*str1)
        return -1; else if (*str2)
        return 1;
    return 0;
}

void strncopy (char *d, const char *r, unsigned int len) {
    unsigned long i;
    for (i = 0; i < len - 1 && r[i] != '\0' ; i++)
        d[i] = r[i];

    d[i] = '\0';
}

/* translate hex format string to unsigned integer */
unsigned long htoui (char *b) {
    if (b[0] != '0' || b[1] != 'x')
        return 0;

    unsigned long num = 0;
    for (int i = 2; b[i] != '\0'; i++) {
        num *= 16;
        if ('0' <= b[i] && b[i] <= '9')
            num += b[i] - '0';
        else if ('a' <= b[i] && b[i] <= 'f')
            num += b[i] - 'a' + 10;
        else if ('A' <= b[i] && b[i] <= 'F')
            num += b[i] - 'A' + 10;
        /* wrong hex format */
        else
            return 0;
    }
    return num;
}

/* translate string to unsigned integer */
unsigned long atoui (char *b) {
    if (b[0] == '0' && b[1] == 'x')
        return htoui(b);

    unsigned long num = 0;
    for (int i = 0; b[i] != '\0'; i++) {
        num *= 10;
        if ('0' <= b[i] && b[i] <= '9')
            num += b[i] - '0';
        /* wrong hex format */
        else
            return 0;
    }
    return num;
}

/* translate string to integer */
long atoi (char *b) {
    if (b[0] != '-')
        return (int)atoui(b);

    long num = 0;
    for (int i = 1; b[i] != '\0'; i++) {
        num *= 10;
        if ('0' <= b[i] && b[i] <= '9')
            num += b[i] - '0';
        /* wrong hex format */
        else
            return 0;
    }
    return -num;
}

long strfind (const char *buffer, char token) {
    for (long i = 0; i < strlen(buffer); i++)
        if (buffer[i] == token)
            return i;
    return -1;
}

void memcpy (void *dest, void *source, unsigned int size) {
    char *ptrd = dest, *ptrs = source;
    for (int i = 0; i < size; i++) {
        ptrd[i] = ptrs[i];
    }
}

long special_token (char *b) {
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

int uitoa (char *dst, unsigned int size, unsigned long num) {
    if (size < 1)
        return 0;
    size -= 1;
    long index = 0;
    unsigned long counter = 1;

    for (;num / counter; counter *= 10) index += 1;
    int i = 0;
    for (counter /= 10; counter && i < size; counter /= 10, i++) {
        int n = num / counter % 10;
        *dst++ = (char)('0' + n);
    }

    dst[i] = '\0';
    return i;
}

int itoa (char *dst, unsigned int size, long num) {
    int i;
    if (size < 2)
        return 0;

    if (num == 0) {
        dst[0] = '0';
        dst[1] = '\0';
        return 1;
    }

    size -= 1;
    if (num < 0) {
        dst[0] = '-';
        i = uitoa(&dst[1], size - 1, -num) + 1;
    }
    else
        i = uitoa(dst, size, num);

    dst[i] = '\0';
    return i;
}

int ftoa (char *dst, unsigned int size, double num) {
    int i = 0;
    if (size < 1)
        return 0;
    size -= 1;

    if (num < 0) {
        dst[0] = '-';
        num = -num;
        i = uitoa(&dst[1], size - 1, (unsigned long)num) + 1;
    }
    else
        i = uitoa(&dst[0], size, (unsigned long)num);
    dst[i] = '.';
    i += 1;

    int counter = 1;
    for (int j = 0; i < size && j < 3; i++, j++) {
        counter *= 10;
        int n = counter * num;
        n %= 10;
        dst[i] = '0' + n;
    }
    dst[i] = '\0';
    return i;
}

int htoa (char *dst, unsigned int size, unsigned long num) {
    if (size < 4)
        return 0;

    size -= 3;
    dst[0] = '0';
    dst[1] = 'x';
    dst += 2;

    long index = 0;
    unsigned long counter = 1;

    for (;num / counter; counter *= 16) index += 1;
    int i = 0;
    for (counter /= 16; counter && i < size; counter /= 16, i++) {
        int n = num / counter % 16;
        if (0 <= n && n <= 9)
            *dst++ = (char)('0' + n);
        else if (10 <= n && n <= 15)
            *dst++ = (char)('a' + n - 10);
    }

    dst[i] = '\0';
    return i + 2;
}

int vsnprintf (char *dst, unsigned int size, char *format, va_list args) {
    unsigned int di = 0;
    unsigned int fi = 0;
    for (; di < size && fi < strlen(format);) {
        long tmp = special_token(&format[fi]);
        if (tmp == -1) {
            strncopy(&dst[di], &format[fi], size - di);
            break;
        }

        unsigned int s = size - di < tmp + 1 ? size - di : tmp + 1;
        strncopy(&dst[di], &format[fi], s);
        di += size - di < tmp ? size - di : tmp;
        fi += tmp;

        if (format[fi] == '\n') {
            strncopy(&dst[di], "\n\r", size - di);
            fi += 1;
            di += 2;
        }
        else if (format[fi] == '%') {
            if (format[fi + 1] == 'd') {
                long v = va_arg(args, int);
                di += itoa(&dst[di], size - di, v);
                fi += 2;
            }
            else if (format[fi + 1] == 'f') {
                double v = va_arg(args, double);
                di += ftoa(&dst[di], size - di, v);
                fi += 2;
            }
            else if (format[fi + 1] == 's') {
                char *v = va_arg(args, char *);
                strncopy(&dst[di], v, size - di);
                di += size - di < strlen(v) ?  size - di : strlen(v);
                fi += 2;
            }
            else if (format[fi + 1] == 'x') {
                unsigned long v = va_arg(args, unsigned long);
                di += htoa(&dst[di], size - di, v);
                fi += 2;
            }
            else if (format[fi + 1] == 'p') {
                void *v = va_arg(args, void *);
                di += htoa(&dst[di], size - di, (unsigned long)v);
                fi += 2;
            }
            // TODO: add other format (ex: %x %c)
        }
        else {
            strncopy(dst, "unknown format: ", size);
            strncopy(&dst[strlen(dst)], format, size - strlen(dst));
        }
    }
    return strlen(dst);
}

int snprintf (char *dst, unsigned int size, char *format, ...) {
    va_list ap;
    va_start(ap, format);
    int i = vsnprintf(dst, size, format, ap);
    va_end(ap);
    return i;
}
