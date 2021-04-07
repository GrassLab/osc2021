#ifndef SHELL_H
#define SHELL_H

#include "utils.h"
#include "uart.h"
#include "system.h"

#define MAX_BUFFER_LEN 128

#define  USER_NAME     "andy"
#define  MACHINE_NAME  "pi"


void shell_welcome();
void shell() ;

#endif