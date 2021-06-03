#ifndef SYS_CALL_H
#define SYS_CALL_H

#include "type.h"

int getpid();
size_t read(char buffer[], size_t size);
size_t write(const char buffer[], size_t size);
int fork();
int exec(const char* name, char * const argv[]);
void exit();

#endif