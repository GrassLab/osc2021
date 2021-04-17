#ifndef SHELL_H
#define SHELL_H

#include "utils.h"
#include "uart.h"
#include "system.h"
#include "irq.h"


#define  USER_NAME     "andy"
#define  MACHINE_NAME  "pi"

char input_buffer[MAX_BUFFER_LEN];
int buffer_index;

void shell_welcome();
void shell() ;

#endif