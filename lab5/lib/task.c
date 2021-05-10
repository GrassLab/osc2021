#include "../include/uart.h"
#include "../include/task.h"
#include "../include/memAlloc.h"
#include "../include/initrd.h"
#include "../include/shell.h"
#include "../include/switch.h"
#include "../include/list.h"
#include "../include/interrupt.h"
#include "../include/stringUtils.h"

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
    new_task->stack_addr = 0;

    RUN_Q_NODE* tmp = (RUN_Q_NODE*)my_alloc(sizeof(RUN_Q_NODE));
    tmp->task = new_task;
    tmp->next = tmp->prev = 0;
    //uart_printf("id:%d\n",tidd);
    //uart_printf("new_task: %x\n", new_task);

    list_push(&run_q,tmp);
    return new_task;

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
            if(tmp->task->stack_addr>=10000000 && tmp->task->stack_addr <=20000000){
            my_free((unsigned long)(tmp->task->stack_addr));
            }
            my_free((unsigned long)(tmp->task));
            my_free((unsigned long)tmp);
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
    task_struct *cur = get_current();

    unsigned long a_addr;
    char buf[BUF_MAX_SIZE];
    uart_puts("please enter app load address:\n");
    read_input(buf);
    a_addr = getHexFromString(buf);
    cur->stack_addr = a_addr;
    unsigned long sp_addr;
    sp_addr = loadprogWithArgv(path, a_addr, argv);
    uart_printf("loadsuce\n");
}

//void ALLMIGHTYLOG(void) {
//    task_struct *cur = get_current();
//    unsigned long *cur_context = cur->context.sp;
//    for(int i = 0 ;i< 35 ; ++i){
//        uart_printf("%x: %x\n", i, cur_context[i]);
//        //uart_printf("%x: %x\n\n", i, new_context[i]);
//    }
//}

void spppp(unsigned long *sp) {
    uart_printf("sp is at: %x\n", sp);
    for(int i = 0 ;i< 35 ; ++i){
        uart_printf("%x: %x\n", i, sp[i]);
        //uart_printf("%x: %x\n\n", i, new_context[i]);
    }
    // char buf[8];
    // read_input(buf);
}

void stop(void) {
    char buf[8];
    task_struct *cur = get_current();
    uart_printf("forked lr: %x\n", cur->context.lr);
    // unsigned long *cur_context = cur->context;
    // for(int i = 0 ;i< 35 ; ++i){
    //     uart_printf("%x: %x\n", i, cur_context[i]);
    //     //uart_printf("%x: %x\n\n", i, new_context[i]);
    // }
    uart_puts("put tido retrun\n");
    // read_input(buf);

    _child_return_from_fork();
}

void sys_fork(trap_frame *tf){
    task_struct *parent = get_current();
    task_struct *child = threadCreate(0);

    int child_id = child->id;
    //uart_printf("child task:%x\n",child);
    char* src = (char*)parent;
    char* dst = (char*)child;
    int iter = TASKSIZE;
    while(iter--){
        *dst = *src;
        src++;
        dst++;
    }
    parent->context.sp =(unsigned long)tf;
    int parent_ustack_size;
    if(child_id == 2){
        parent_ustack_size = (parent->stack_addr)-(tf->sp_el0);
    }else if(child_id == 3){
        parent_ustack_size = (parent->stack_addr)-(tf->sp_el0);
    }
    uart_printf("parent_ustack_addr:%x\n", parent -> stack_addr);
    uart_printf("parent_sp_el0:%x\n", tf -> sp_el0);
    uart_printf("parent ustack size:%x\n", parent_ustack_size);
    if ((unsigned long)child > (unsigned long)parent) {
        child->context.sp = parent->context.sp + ((unsigned long)child - (unsigned long)parent);
    } else {
        child->context.sp = parent->context.sp - ((unsigned long)parent - (unsigned long)child);
    }
//    uart_printf("child sp: %x\n", child->context.sp);
    child->context.fp =(unsigned long)(child+TASKSIZE);
    child->context.lr =(unsigned long)_child_return_from_fork;
    child->id = child_id;
    trap_frame *child_tf =(trap_frame*)(child->context.sp);
    //uart_puts("Please enter child ustack address\n");
    //char buf[128];
    //read_input(buf);
    //unsigned long child_ustack = getHexFromString(buf);

    unsigned long child_ustack = my_alloc(4096) + 4096;

    char *src_stack = (char*)(tf->sp_el0);
    char *dst_stack = (char*)(child_ustack - parent_ustack_size);
    child -> stack_addr = child_ustack;

    child_tf->sp_el0 = child_ustack - parent_ustack_size;
    //uart_printf("child sp_el0: %x\n",child_tf->sp_el0);
    child_tf->regs[0] = 0;
    child_tf->regs[29] = child_ustack;
    tf->regs[0] = child->id;
    //uart_printf("%x\n",tf->sp_el0);
    //uart_printf("%x\n",parent_ustack);
    while(parent_ustack_size--){
        *dst_stack = *src_stack;
        src_stack++;
        dst_stack++;
    }
    return;

}

int getpid(trap_frame* tf){
    task_struct *cur = get_current();
    tf->regs[0] = cur->id;
}
void foo1(){
    task_struct *cur = get_current();
    for(int i = 0; i<2 ; ++i){
        uart_printf("Thread id:%d, loop:%d\n",cur->id,i);
//        for(int j =0 ; j < (1<<27);++j){}
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
    run_q.beg = run_q.end = 0;
    exit_q.beg = exit_q.end = 0;

    task_struct* root_task = threadCreate(idle);
    asm volatile("msr tpidr_el1, %0\n" ::"r"(root_task));

    threadCreate(foo1);
    threadCreate(foo1);
    threadCreate(foo1);
    idle();

}

void test2(){
    tidd = 0;
    run_q.beg = run_q.end = 0;
    exit_q.beg = exit_q.end = 0;
    task_struct* root_task = threadCreate(idle);
    asm volatile("msr tpidr_el1, %0\n" ::"r"(root_task));

    threadCreate(foo2);
    idle();
}
