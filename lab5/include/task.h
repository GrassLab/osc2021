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
  unsigned long sp;
  unsigned long pc;//x30
}cpu_context;

typedef struct _task_struct{
  cpu_context context;
  int id;
  int state;
  unsigned long a_addr, a_size, child;
  struct _task_struct *next;
}task_struct;

typedef struct _RUN_Q{

  task_struct *beg, *end;
}RUN_Q

#endif
