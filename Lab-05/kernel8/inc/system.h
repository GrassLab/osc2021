#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include "uart.h"
#include "thread.h"

#define SYS_CMD_NUM 16

void system_command(char*);

void help();
void hello();



#endif
