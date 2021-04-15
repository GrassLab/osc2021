#include "sched.h"

struct task_struct init_task = INIT_TASK;
struct task_struct *current = &(init_task);

int pid_count = 1;



void add_task(){
	task_struct *new_task =(task_struct *)kmalloc(sizeof(task_struct));
	new_task->state = 0;
	new_task->priority = 0;
    new_task->preempt = 0;
    new_task->pid = pid_count;	
	pid_count++;
	new_task->next = NULL;
	current = new_task;
}

void setpriority(char* args) { 
	int priority = 0;
	for(int i = 0; args[i] != '\0'; ++i){
		if(args[i] >= '0' && args[i] <= '9') priority = priority * 10 + args[i] - '0';
		else priority = 0;
	}
	current->priority = priority;
}
void preempt(char* args) { 
	int preempt = 0;
	for(int i = 0; args[i] != '\0'; ++i){
		if(args[i] >= '0' && args[i] <= '9') preempt = preempt * 10 + args[i] - '0';
		else preempt = 0;
	}
	current->preempt = preempt;
}



void getpid() { 
	uart_put_int(current->pid);
	uart_puts("\n");
}
