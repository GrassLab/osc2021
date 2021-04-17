#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>

#define pad(x, y) ((((x) + (y)-1) / (y)) * (y))
#define max(x, y) (x > y ? x : y)
#define min(x, y) (x < y ? x : y)

int strcmp(const char *s1, const char *s2);
int strcmp_n(const char *s1, const char *s2, size_t n);
void *memcpy(void *dst, const void *src, size_t len);
char *strcpy(char *dst, const char *src);
char *strcpy_n(char *dst, const char *src, size_t len);
size_t strlen(const char *str);

long atoi(const char *s);
long atoi_n(const char *s, size_t len, size_t base);
size_t atol(const char *s);
size_t atol_n(const char *s, size_t len, size_t base);

char *new_str(char *src);
char *split_str(char *str);

// circular double linked list
typedef struct cdl_list {
  struct cdl_list *fd;
  struct cdl_list *bk;
} cdl_list;

static inline void init_cdl_list(cdl_list *l) {
  l->bk = l;
  l->fd = l;
}

static inline void push_cdl_list(cdl_list *l, cdl_list *chunk) {
  chunk->bk = l;
  chunk->fd = l->fd;
  l->fd->bk = chunk;
  l->fd = chunk;
}

static inline void *pop_cdl_list(cdl_list *chunk) {
  chunk->fd->bk = chunk->bk;
  chunk->bk->fd = chunk->fd;
  return (void *)chunk;
}

static inline int cdl_list_empty(cdl_list *l) { return l->fd == l; }

// linked list
typedef struct l_list {
  struct l_list *next;
} l_list;

static inline void init_l_list(l_list *l) { l->next = NULL; }

static inline void push_l_list(l_list *l, l_list *chunk) {
  chunk->next = l->next;
  l->next = chunk;
}

static inline void *pop_l_list(l_list *l) {
  void *addr = (void *)l->next;
  l->next = l->next->next;
  return addr;
}

static inline int l_list_empty(l_list *l) { return l->next == NULL; }

#endif