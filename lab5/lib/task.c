#include "../include/uart.h"
#include "../include/task.h"
#include "../include/memAlloc.h"
#include "../include/initrd.h"
#include "../include/shell.h"

#define TASKSIZE 4096

static RUN_Q run_q;
static int task_count;

task_struct* threadCreate(void *func){

    task_struct *new_task = (task_struct *)my_alloc(TASKSIZE);

    new_task ->  



}

void idle(){
    while(1){
        zombiekill();
        threadSchedule();
    }

}
