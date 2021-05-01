#ifndef SHELL_H
#define SHELL_H

#include "uart.h"
#include "system.h"


#define MAX_BUFFER_LEN 128
#define  USER_NAME     "andy"
#define  MACHINE_NAME  "pi"

char input_buffer[MAX_BUFFER_LEN];
int buffer_index;

void shell_welcome();
void shell() ;

#endif