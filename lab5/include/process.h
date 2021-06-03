#ifndef __PROCESS__
#define __PROCESS__

#define MAX_PID_NUM 65536

void init_thread();
void thread_create(void (*thread)());
int exec(const char* name, char *const argv[]);
void exit();
int fork();
void kill_zombies();

#endif