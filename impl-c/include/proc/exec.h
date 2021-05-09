#pragma once
#include <stddef.h>
void *load_program(const char *name, /*OUT*/ size_t *target_size);
int exec(const char *name, char *const argv[]);
