#ifndef __STRING_H
#define __STRING_H

#include <stdint.h>
#include <stddef.h>

#define NEW_LINE "\r\n"
#define TAB "    "

int strcmp(const char *X, const char *Y);
int strncmp(const char *X, const char *Y, size_t n);
char *strtok(char *str, char delim);
void strrev(char *head);
char *trim(char *head);
uint32_t xatou32(const char *str, size_t len);
int uitoa(uint32_t num, char *buffer, size_t buffer_size, uint32_t base);

#endif
