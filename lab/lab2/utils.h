#ifndef _UTILS
#define _UTILS

extern void _moveTo(void *, unsigned int);
extern void _branch(void *);
char *itoa(int num, char *str) {
  int i = 0;
  char buff[64];
  for (; num; i++, num /= 10) buff[i] = '0' + num % 10;
  str[i] = '\0';
  for (int j = 0; j < i; j++) str[j] = buff[i - j - 1];
  return str;
}

#endif /*_UTILS */
