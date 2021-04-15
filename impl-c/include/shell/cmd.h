#pragma once

typedef struct {
  char *name;
  char *help;
  void (*func)(void);
} Cmd;

Cmd *getCmd(char *name);

// Only used for running tests
void test_shell_cmd();