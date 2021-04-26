#pragma once

void _exec_usr(void *addr, void *sp, unsigned long pstate);
void exec(const char *name, int argc, const char **argv);