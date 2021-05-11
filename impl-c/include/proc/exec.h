#pragma once
#include <stddef.h>
void *load_program(const char *name, /*OUT*/ size_t *target_size);
int exec(const char *name, char *const argv[]);

// Create and bind a user thread to current running task.
// Exec as user code
void exec_user(const char *name, char **argv);