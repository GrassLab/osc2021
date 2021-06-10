#ifndef __PRINTF_H
#define __PRINTF_H

#include <stdint.h>
#include <stdarg.h>
#include "sys_call.h"

#define PRINTF_BUF_SIZE 512

static uint32_t printf_append_C(char c);
static uint32_t printf_append_S(const char *s);
static uint32_t printf_number(uint64_t num, uint8_t base);
static uint32_t printf_d(const char *fmt, va_list args, uint32_t len);
static uint32_t printf_ld(const char *fmt, va_list args, uint32_t len);
static uint32_t printf_u(const char *fmt, va_list args, uint32_t len);
static uint32_t printf_lu(const char *fmt, va_list args, uint32_t len);
static uint32_t printf_x(const char *fmt, va_list args, uint32_t len);
static uint32_t printf_lx(const char *fmt, va_list args, uint32_t len);
static uint32_t printf_c(const char *fmt, va_list args, uint32_t len);
static uint32_t printf_s(const char *fmt, va_list args, uint32_t len);
static uint32_t vprintf(const char *fmt, va_list args, uint32_t len);
size_t printf(const char *fmt, ...);
size_t puts(const char *str);

#endif
