#include "stddef.h"

typedef unsigned long size_t;
void *malloc(size_t n);
void *calloc(size_t n, size_t sz);
void free(void *mptr);
int abs(int n);
