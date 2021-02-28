#ifndef _SHELL_H
#define _SHELL_H

void buffer_clear();
void execute_command(char *);
void init_shell();

void print_input_prompt();
void get_input();

void simple_shell();

void shell_hello();
void shell_help();

#endif