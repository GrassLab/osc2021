#ifndef _TASK_H
#define _TASK_H
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
}task_struct;


typedef struct _RUN_Q_NODE{
  task_struct* task;
  struct _RUN_Q_NODE *next;
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
#endif
