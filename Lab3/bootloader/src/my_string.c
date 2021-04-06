
#include "my_math.h"
#include "stddef.h"
char *itoa(int value, char *s){
	int idx = 0;
	int tidx = 0;
	char tmp[10];
	if(value < 0){
		s[idx++] = '-';
		value *= -1;
	}
	do {
        tmp[tidx++] = '0' + value % 10;
        value /= 10;
    } while (value != 0 && tidx < 11);
	for(int i = tidx-1; i >= 0 ; i--){
		s[idx++] = tmp[i];
	}
	s[idx] = '\0';
	return s;
}

char *ftoa(float value, char *s){
	int first;
	float second;
	int idx = 0;
	if(value < 0){
		s[idx++] = '-';
		value *= -1;
	}
	first = (int) value;
	second = value - first;
	int f_int = (int)second * 1000000;
	char i_char[11];
	char f_char[7];
	itoa(first, i_char);
	itoa(f_int, f_char);
	char *ptr = i_char;
    while (*ptr) s[idx++] = *ptr++;
    s[idx++] = '.';
    // copy float part
    ptr = f_char;
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

int strncmp(const char *a, const char *b, int n) {
  size_t i = 0;
  while (i < n-1 && a[i] == b[i] && a[i] != '\0' && b[i] != '\0') i++;
  return a[i] - b[i];
}
size_t strlen(const char *s) {
  size_t i = 0;
  while (s[i]) i++;
  return i;
}
