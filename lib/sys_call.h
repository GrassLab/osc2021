#ifndef SYS_CALL_H
#define SYS_CALL_H

#include "type.h"

int getpid();
size_t read_buffer(char buffer[], size_t size);
size_t print(const char buffer[], size_t size);
int fork();
int exec(const char* name, char * const argv[]);
void exit();

int open(const char * path_name, int flags);
int close(int fd);
int write(int fd, const void * buf, size_t len);
int read(int fd, void * buf, size_t len);

#endif