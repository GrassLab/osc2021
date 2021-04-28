#include "../include/uart.h"
#include "../include/task.h"
#include "../include/memAlloc.h"
#include "../include/initrd.h"
#include "../include/shell.h"
#include "../include/switch.h"
#include "../include/list.h"
#include "../include/interrupt.h"


#define BUF_MAX_SIZE 128
#define TASKSIZE 4096
#define TASK_ALIVE 1
#define TASK_DEAD 2

static RUN_Q run_q;
static RUN_Q exit_q;
static int tidd = 0;

void dump_q(RUN_Q* q){
    for(RUN_Q_NODE* i = q->beg;i!=0 ; i = i->next){
        uart_printf("%x\n",i->task);
    }
}

void list_push(RUN_Q* q,RUN_Q_NODE *tmp){
    if(q->beg == 0){
        q->beg = q->end = tmp;
    }else{
        //tmp->prev = q->end;
        //tmp->next = q->beg;
        //q->end->next = tmp;
        //q->beg->prev = tmp;
        //q->end = tmp;
        q->end->next = tmp;
        tmp->prev = q->end;
        q->end = tmp;
    }

}

RUN_Q_NODE* list_pop(RUN_Q *q){
    if((q->beg) == 0){
        return 0;
    }else{
        if((q->beg->next) == 0){
            RUN_Q_NODE *tmp = q->beg;
            q->beg = q->end = 0;
            tmp->next = tmp->prev = 0;
            return tmp;
        }else{
            RUN_Q_NODE *tmp = q->beg;
            q->beg = q->beg->next;
            q->beg->prev = 0;
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
    new_task->id = tidd++;

    RUN_Q_NODE* tmp = (RUN_Q_NODE*)my_alloc(sizeof(RUN_Q_NODE));
    tmp->task = new_task;
    tmp->next = tmp->prev = 0;
    //uart_printf("id:%d\n",tidd);

    list_push(&run_q,tmp);
    return new_task;

}

void threadSchedule(){
    RUN_Q_NODE *next_node = list_pop(&run_q);
    task_struct *cur = get_current();

    //uart_printf("cur task:%x\n",cur);
    //uart_printf("next id:%d\n",next_node->task->id);
    //uart_printf("next task:%x\n",next_node->task);

    if(next_node){
        if((next_node->task->state) == TASK_ALIVE){
            list_push(&run_q,next_node);
    //uart_printf("cur id:%x\n",cur->id);
    //uart_printf("next id:%x\n",next_node->task->id);
    //uart_printf("task size:%x\n",sizeof(task_struct));
    //uart_printf("context size:%x\n",sizeof(cpu_context));
    //uart_puts("enter to resume\n");
    //char buf[100];
    //read_input(buf);
            //uart_puts("runq:\n");
            //dump_q(&run_q);
            switch_to(cur,next_node->task);
    //uart_printf("test id:%d\n",next_node->task->id);


        }else{
            list_push(&exit_q,next_node);
            //uart_puts("runq:\n");
            //dump_q(&run_q);
            //uart_puts("exitq:\n");
            //dump_q(&exit_q);
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
    threadSchedule();
}

void idle(){
    //uart_printf("in idle\n");
    while(1){
        zombiekill();
        threadSchedule();
    }
}

void exec(char *path, char** argv){
    unsigned long a_addr;
    char buf[BUF_MAX_SIZE];
    uart_puts("please enter app load address:\n");
    read_input(buf);
    a_addr = getHexFromString(buf);
    unsigned long sp_addr;
    sp_addr = loadprogWithArgv(path, a_addr, argv);
    uart_printf("loadsuce\n");
    task_struct *cur = get_current();
    cur->context.lr = a_addr;

    asm volatile("mov x0, 0x340   \n"::);
    asm volatile("msr spsr_el1, x0   \n"::);
    asm volatile("msr elr_el1, %0   \n"::"r"(cur->context.lr));
    asm volatile("msr sp_el0, %0   \n"::"r"(sp_addr));
    core_timer_enable();
    asm volatile("eret   \n");
}

int getpid(trap_frame* tf){
    task_struct *cur = get_current();
    tf->regs[0] = cur->id;
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

void foo2(){
    char* argv[] = {"argv_test", "-o", "arg2", 0};
    exec("app1", argv);
}

void test1(){
    tidd = 0;
 //   uart_printf("tidd:%d\n",tidd);
    run_q.beg = run_q.end = 0;
    exit_q.beg = exit_q.end = 0;

    task_struct* root_task = threadCreate(idle);
    asm volatile("msr tpidr_el1, %0\n" ::"r"(root_task));

//    threadCreate(foo2);
    threadCreate(foo1);
    threadCreate(foo1);
    threadCreate(foo1);
    //dump_q(&run_q);
    idle();

}

void test2(){
    tidd = 0;
    run_q.beg = run_q.end = 0;
    exit_q.beg = exit_q.end = 0;
    task_struct* root_task = threadCreate(idle);
    asm volatile("msr tpidr_el1, %0\n" ::"r"(root_task));

    threadCreate(foo2);
    threadCreate(foo1);
    threadCreate(foo1);
    threadCreate(foo1);
    idle();
}
