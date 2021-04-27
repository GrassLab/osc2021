#include "../include/uart.h"
#include "../include/task.h"
#include "../include/memAlloc.h"
#include "../include/initrd.h"
#include "../include/shell.h"
#include "../include/switch.h"
#include "../include/list.h"


#define BUF_MAX_SIZE 128
#define TASKSIZE 4096
#define TASK_ALIVE 1
#define TASK_DEAD 2

static RUN_Q run_q;
static RUN_Q exit_q;
static int tid = 0;

void dump_q(RUN_Q* q){
    for(RUN_Q_NODE* i = q->beg; i!=q->end; i = i->next){
        uart_printf("%x\n",i->task);
    }
}

void list_push(RUN_Q* q,RUN_Q_NODE *tmp){
    if(q->beg == 0){
        q->beg = q->end = tmp;
    }else{
        tmp->prev = q->end;
        tmp->next = q->beg;
        q->end->next = tmp;
        q->beg->prev = tmp;
        q->end = tmp;
    }

}

RUN_Q_NODE* list_pop(RUN_Q *q){
    if((q->beg) == 0){
        return 0;
    }else{
        if((q->beg->next) == q){
            RUN_Q_NODE *tmp = q->beg;
            q->beg = q->end = 0;
            tmp->next = tmp->prev = 0;
            return tmp;
        }else{
            RUN_Q_NODE *tmp = q->beg;
            q->beg = q->beg->next;
            q->beg->prev = q->end;
            q->end->next = q->beg;
            tmp->next= 0;
            tmp->prev = 0;
            return tmp;
        }
    }

}

task_struct* threadCreate(void *func){
    task_struct* new_task = (task_struct*)my_alloc(TASKSIZE);
    new_task->context.fp = (unsigned long)new_task+TASKSIZE;
    new_task->context.lr = (unsigned long)func;
    new_task->context.sp = (unsigned long)new_task+TASKSIZE;

    new_task->state = TASK_ALIVE;
    new_task->id = tid++;

    RUN_Q_NODE* tmp = (RUN_Q_NODE*)my_alloc(sizeof(RUN_Q_NODE));
    tmp->task = new_task;
    tmp->next = tmp->prev = new_task;

    list_push(&run_q,tmp);

}

void threadSchedule(){
    RUN_Q_NODE *next_node = list_pop(&run_q);
    task_struct *cur = get_current();

    if(next_node){
        if((next_node->task->state) == TASK_ALIVE){
            list_push(&run_q,next_node);
            switch_to(cur,next_node->task);
        }else{
            list_push(&exit_q,next_node);
        }
    }

}

void zombiekill(){
    while(1){
        RUN_Q_NODE *tmp = list_pop(&exit_q);
        if(tmp){
            my_free(tmp->task);
            my_free(tmp);
        }else{
            return;
        }
    }
}

void cur_exit(){
    task_struct *cur = get_current();
    cur->state = TASK_DEAD;
    //threadSchedule();
}

void idle(){
    while(1){
        zombiekill();
        threadSchedule();
    }
}

void foo1(){
    task_struct *cur = get_current();
    for(int i = 0; i<2 ; ++i){
        uart_printf("Thread id:%d, loop:%d\n",cur->id,i);
        for(int j =0 ; j < (1<<27);++j){}
        threadSchedule();
    }
    cur_exit();
}

void test1(){

    run_q.beg = run_q.end = 0;
    exit_q.beg = exit_q.end = 0;

    //threadCreate(idle);
    threadCreate(foo1);
    threadCreate(foo1);
    threadCreate(foo1);
    //dump_q(&run_q);
    idle();


}
