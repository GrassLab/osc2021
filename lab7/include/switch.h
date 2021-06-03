#ifndef _SWITCH_H
#define _SWITCH_H

void switch_to(task_struct* prev, task_struct *next);
task_struct*  get_current();

#endif

