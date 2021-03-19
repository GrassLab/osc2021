#pragma once

#include <stdint.h>
#include <stddef.h>

void memcpy(void *dest, const void * src, size_t n);

int strcmp(const char *, const char *);

char* itoa(int64_t val, int base);