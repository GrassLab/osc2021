#ifndef _COMMAND_H
#define _COMMAND_H

#define MAX_COMMAND_SIZE (20)

void fetch(char *command, int maxSize);
void exec_command(char *input);

void reboot(int tick);

#endif