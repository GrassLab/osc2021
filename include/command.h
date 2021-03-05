#ifndef _COMMAND_H
#define _COMMAND_H

void exec_command(char *input);
void reboot(int tick);
void fetch(char *command, int maxSize);

#endif