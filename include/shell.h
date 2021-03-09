#ifndef __SHELL_H__
#define __SHELL_H__

#include "system.h"
#include "uart.h"
#define INPUT_SIZE 64

char input_buffer[64];
void shell();
void shell_welcome();

#endif