#ifndef SHELL_H
#define SHELL_H
#include <string.h>
#include "reset.h"
#include <uart.h>
#include <types.h>
void shell();
void do_command(char* command);
void loadimg(size_t load_address);
#endif
