#pragma once

typedef struct {
  char *name;
  char *help;
  void (*func)(void);
} Cmd;

Cmd *getCmd(char *name);