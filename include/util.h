#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>

#include "bl_util.h"

#define pad(x, y) ((((x) + (y)-1) / (y)) * (y))

int strcmp(const char *p1, const char *p2);
size_t strlen(const char *str);
int strcmp_n(const char *s1, const char *s2, size_t n);
void *memcpy(void *dst, const void *src, size_t len);
char *strcpy(char *dst, const char *src);
char *strcpy_n(char *dst, const char *src, size_t len);

long long atoi(const char *s);
unsigned long long atol(const char *s);
long long atoi_n(const char *s, size_t len, int base);
unsigned long long atol_n(const char *s, size_t len, int base);

char *new_str(char *src);

typedef struct list_head {
  struct list_head *fd;
  struct list_head *bk;
} list_head;

void init_list(list_head *l);
void push_list(list_head *l, list_head *chunk);
void pop_list(list_head *chunk);
int list_empty(list_head *l);

#endif