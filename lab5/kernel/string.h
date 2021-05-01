#ifndef __STRING_H
#define __STRING_H

#include <stdint.h>
#include <stddef.h>

#define NEW_LINE "\r\n"
#define TAB "    "

size_t strlen(const char *str);
void strncpy(char *to, const char *from, int64_t n);
void strfill(char *to, char from, size_t n);
int32_t strcmp(const char *X, const char *Y);
uint32_t strncmp(const char *X, const char *Y, size_t n);
char *strtok(char *str, char delim);
void strrev(char *head);
char *trim(char *head);
uint32_t xatou32(const char *str, size_t len);
uint32_t uitoa(uint32_t num, char *buffer, size_t buffer_size, uint8_t base);

#endif
