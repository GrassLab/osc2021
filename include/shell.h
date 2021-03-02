#pragma once

#define MAX_BUFFER_SIZE 64
char buffer[MAX_BUFFER_SIZE];
int buffer_pos;

void cmd_help();
void cmd_hello();
void clear_buffer();
void receive_cmd();
void run_shell();
