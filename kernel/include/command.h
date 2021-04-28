#ifndef _COMMAND_H
#define _COMMAND_H

#define MAX_COMMAND_SIZE (20)

void get(char *command, int maxSize);
void exec_command(char *input);

void reboot(int tick);
void demo1();
void demo2();
void foo();
#endif