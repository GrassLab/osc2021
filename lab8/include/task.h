#ifndef _TASK_H
#define _TASK_H

#include "vfs.h"

typedef struct _trap_frame{
  unsigned long regs[32];
  unsigned long sp_el0;
  unsigned long elr_el1;
  unsigned long spsr_el1;
}trap_frame;

typedef struct _cpu_context{
  unsigned long x19;
  unsigned long x20;
  unsigned long x21;
  unsigned long x22;
  unsigned long x23;
  unsigned long x24;
  unsigned long x25;
  unsigned long x26;
  unsigned long x27;
  unsigned long x28;
  unsigned long fp;//x29
  unsigned long lr;//x30
  unsigned long sp;
}cpu_context;

typedef struct _task_struct{
  cpu_context context;
  int id;
  int state;
  unsigned long stack_addr;
  unsigned long a_addr;
  file* fd_table[5];
  void* page_table;
}task_struct;


typedef struct _RUN_Q_NODE{
  task_struct* task;
  struct _RUN_Q_NODE *next;
  struct _RUN_Q_NODE *prev;
}RUN_Q_NODE;

typedef struct _RUN_Q{

  RUN_Q_NODE *beg;
  RUN_Q_NODE *end;
}RUN_Q;

task_struct* threadCreate(void* func);
void foo();
void cur_exit();
void threadSchedule();
void idle();
int getpid();
void test3();
int sys_open(char* pathname,int flags);
int sys_close(int fd);
int sys_write(int fd, char* buf, int count);
int sys_read(int fd, char* buf, int count);

void _child_return_from_fork(void);
#endif
