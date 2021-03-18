#ifndef COMMAND_H
#define COMMAND_H

void InputBufferOverflowMessage(char []);

void CommandHelp();
void CommandHello();
void CommandCpiols();
void CommandCpiocat(char *);
void CommandTimestamp();
void CommandNotFound(char *);
void CommandReboot();

#endif
