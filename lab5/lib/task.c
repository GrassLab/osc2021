#include "../include/uart.h"
#include "../include/task.h"
#include "../include/memAlloc.h"
#include "../include/initrd.h"
#include "../include/shell.h"
#include "../include/switch.h"

#define TASKSIZE 4096
#define ALIVE 1
#define DEAD 2

static RUN_Q run_q;
static int tid = 0;

void dump_run_q(){
    RUN_Q_NODE* tmp = run_q.beg;
    do{
        uart_printf("%x\n",tmp -> task);
        tmp = tmp->next ;
    }while((tmp -> next)!=0);


}
task_struct* threadCreate(void *func){

    task_struct *new_task = (task_struct *)my_alloc(TASKSIZE);

    new_task->context.fp = (unsigned long)new_task + TASKSIZE;
    new_task->context.sp = (unsigned long)new_task + TASKSIZE;
    new_task->context.lr = (unsigned long)func;
    new_task -> state = ALIVE;
    new_task -> id = tid++;


    RUN_Q_NODE *tmp = my_alloc(sizeof(RUN_Q_NODE));
    tmp -> task = new_task;
    tmp -> next = 0;

    if(run_q.beg == 0){
        run_q.beg = tmp;
        run_q.end = tmp;
    }else{
        run_q.end -> next = tmp;
        run_q.end = tmp;
    }

}

void foo(){
    task_struct *cur = get_current();
    for(int i = 0; i < 2; ++i) {
        uart_printf("Thread id: %d loop:%d\n", cur -> id,i );
        for(int j = 0; j < (1<<27); ++j){
        }
        threadSchedule();
    }
    cur_exit();
}
void cur_exit(){
    task_struct *cur = get_current();
    cur -> state = DEAD;
    threadSchedule();
}

void threadSchedule(){

    task_struct *cur = get_current();

    task_struct *next_task = 0;
    RUN_Q_NODE *next_node;
    while(1){
        if(run_q.beg != NULL){
            next_node = run_q.beg;
            if((next_node -> task -> state) == ALIVE){
                next_task = next_node ->task;
                run_q.end -> next = run_q.beg;
                run_q.beg = run_q.beg -> next;
                run_q.end = run_q.end -> next;
                run_q.end -> next = 0;
                break;
            }else if((next_node -> task -> state) == DEAD){
                run_q.beg = run_q.beg -> next;
                my_free(next_node -> task);
                my_free(next_node);
            }
        }
         if(run_q.beg == 0){
            break;
         }
    }
    if(next_task != 0){
        switch_to(cur,next_task);

    }else{
        uart_puts("no more task needs to be handled\n");
        while(1){}
    }

}


void idle(){
    while(1){

        threadSchedule();
    }
}
void test(){
    run_q.beg = run_q.end = 0;
    //asm volatile("msr tpidr_el1, %0\n" ::"r"((unsigned long)root_task));
    threadCreate(foo);
    threadCreate(foo);
    threadCreate(foo);
    dump_run_q();
    idle();
}
