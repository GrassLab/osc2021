#ifndef __UTIL_H_
#define __UTIL_H_

#ifdef _QEMU
#define KERNEL_ADDR 0x80000
#else
#define KERNEL_ADDR 0x80000
#endif // _QEMU


char *gets(char *);
int puts(const char *);
int strcmp(const char *, const char *);

#endif // __UTIL_H_
