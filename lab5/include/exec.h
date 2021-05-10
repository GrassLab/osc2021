#ifndef __EXEC_H_
#define __EXEC_H_

int do_exec(const char *path, const char *argv[]);
int kernel_exec_file(const char *path, const char *argv[]);

#endif