

char *itoa(int value, char *s) {
    int idx = 0;
    if (value < 0) {
        value *= -1;
        s[idx++] = '-';
    }

    char tmp[10];
    int tidx = 0;
    do {
        tmp[tidx++] = '0' + value % 10;
        value /= 10;
    } while (value != 0 && tidx < 11);

    // reverse tmp
    int i;
    for (i = tidx - 1; i >= 0; i--) {
        s[idx++] = tmp[i];
    }
    s[idx] = '\0';

    return s;
}

char *ftoa(float value, char *s) {
    int idx = 0;
    if (value < 0) {
        value = -value;
        s[idx++] = '-';
    }

    int ipart = (int)value;
    float fpart = value - (float)ipart;

    // convert ipart
    char istr[11];  // 10 digit
    itoa(ipart, istr);

    // convert fpart
    char fstr[7];  // 6 digit
    fpart *= (int)pow(10, 6);
    itoa((int)fpart, fstr);

    // copy int part
    char *ptr = istr;
    while (*ptr) s[idx++] = *ptr++;
    s[idx++] = '.';
    // copy float part
    ptr = fstr;
    while (*ptr) s[idx++] = *ptr++;
    s[idx] = '\0';

    return s;
}

unsigned int vsprintf(char *dst, char *fmt, __builtin_va_list args) {
    char *dst_orig = dst;

    while (*fmt) {
        if (*fmt == '%') {
            fmt++;
            // escape %
            if (*fmt == '%') {
                goto put;
            }
            // string
            if (*fmt == 's') {
                char *p = __builtin_va_arg(args, char *);
                while (*p) {
                    *dst++ = *p++;
                }
            }
            // number
            if (*fmt == 'd') {
                int arg = __builtin_va_arg(args, int);
                char buf[11];
                char *p = itoa(arg, buf);
                while (*p) {
                    *dst++ = *p++;
                }
            }
            // float
            if (*fmt == 'f') {
                float arg = (float) __builtin_va_arg(args, double);
                char buf[19];  // sign + 10 int + dot + 6 float
                char *p = ftoa(arg, buf);
                while (*p) {
                    *dst++ = *p++;
                }
            }
        } else {
        put:
            *dst++ = *fmt;
        }
        fmt++;
    }
    *dst = '\0';

    return dst - dst_orig;  // return written bytes
}

unsigned int sprintf(char *dst, char *fmt, ...) {
    __builtin_va_list args;
    __builtin_va_start(args, fmt);
    return vsprintf(dst, fmt, args);
}

int strcmp(const char *X, const char *Y) {
    while (*X) {
        if (*X != *Y)
            break;
        X++;
        Y++;
    }
    return *(const unsigned char *)X - *(const unsigned char *)Y;
}
