#include "string.h"

/* Compare S1 and S2, returning less than, equal to or
   greater than zero if S1 is lexicographically less than,
   equal to or greater than S2.  */
int strcmp(const char *p1, const char *p2) {
  const unsigned char *s1 = (const unsigned char *)p1;
  const unsigned char *s2 = (const unsigned char *)p2;
  unsigned char c1, c2;
  do {
    c1 = (unsigned char)*s1++;
    c2 = (unsigned char)*s2++;
    if (c1 == '\0') return c1 - c2;
  } while (c1 == c2);
  return c1 - c2;
}

/* Compare no more than N characters of S1 and S2,
   returning less than, equal to or greater than zero
   if S1 is lexicographically less than, equal to or
   greater than S2.  */
int strncmp(const char *s1, const char *s2, unsigned n) {
  unsigned char c1 = '\0';
  unsigned char c2 = '\0';
  if (n >= 4) {
    unsigned n4 = n >> 2;
    do {
      c1 = (unsigned char)*s1++;
      c2 = (unsigned char)*s2++;
      if (c1 == '\0' || c1 != c2) return c1 - c2;
      c1 = (unsigned char)*s1++;
      c2 = (unsigned char)*s2++;
      if (c1 == '\0' || c1 != c2) return c1 - c2;
      c1 = (unsigned char)*s1++;
      c2 = (unsigned char)*s2++;
      if (c1 == '\0' || c1 != c2) return c1 - c2;
      c1 = (unsigned char)*s1++;
      c2 = (unsigned char)*s2++;
      if (c1 == '\0' || c1 != c2) return c1 - c2;
    } while (--n4 > 0);
    n &= 3;
  }
  while (n > 0) {
    c1 = (unsigned char)*s1++;
    c2 = (unsigned char)*s2++;
    if (c1 == '\0' || c1 != c2) return c1 - c2;
    n--;
  }
  return c1 - c2;
}

int strlen(const char *s) {
  int len = 0;
  while (s[len] != '\0') {
    len++;
  }
  return len;
}

/*
 * Find the first occurrence of find in s.
 */
char *strstr(const char *s, const char *find) {
  char c, sc;
  int len;
  if ((c = *find++) != 0) {
    len = strlen(find);
    do {
      do {
        if ((sc = *s++) == 0) return 0;
      } while (sc != c);
    } while (strncmp(s, find, len) != 0);
    s--;
  }
  return ((char *)s);
}

char *strcpy(char *dst, const char *src) {
  // return if no memory is allocated to the destination
  if (dst == 0) return 0;

  char *ptr = dst;
  while (*src != '\0') {
    *dst = *src;
    dst++;
    src++;
  }
  *dst = '\0';
  return ptr;
}

char *strncpy(char *dst, const char *src, size_t len) {
  // return if no memory is allocated to the destination
  if (dst == 0) return 0;

  // take a pointer pointing to the beginning of destination string
  char *ptr = dst;

  // copy the C-string pointed by source into the array
  // pointed by destination
  while (*src != ' ') {
    *dst = *src;
    dst++;
    src++;
    len--;
    if (!len) {
      break;
    }
  }

  // include the terminating null character
  *dst = '\0';

  // destination is returned by standard strcpy()
  return ptr;
}

void strcat(char *to, const char *from) {
  while (*to) {
    to++;
  }
  while (*from) {
    *to = *from;
    to++;
    from++;
  }
  *to = '\0';
}

char *strtok(char *s, const char delim) {
  static char *pos;
  char *ret;
  if (s) pos = s;

  if (*pos == '\0') return 0;
  // skip leading
  while (*pos == delim) {
    pos++;
  }

  ret = pos;
  while (*pos != delim && *pos != '\0') {
    pos++;
  }
  if (*pos != '\0') {
    *pos = '\0';
    pos++;
  }
  return ret;
}

char *split_last(char *str, char delim) {
  char *mid = 0;
  while (*str) {
    if (*str == delim) {
      mid = str;
    }
    str++;
  }
  if (mid) {
    *mid = '\0';
    mid++;
  }
  return mid;
}