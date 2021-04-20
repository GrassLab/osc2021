#include "thread.h"
#include "uart.h"
#include "allocator.h"

#define TASKSIZE 4096
#define TASKEXIT 1

typedef struct _Task{
	unsigned long context[13];
	int id;
	int status;
	struct _Task* next;
	/*
	task stack:this ~ this+TASKSIZE
	*/
}Task;

typedef struct{
	Task *beg,*end;
}RQ;

static RQ rq;
static int task_cnter;

void threadSwitch(){
	asm volatile("\
_threadSwitch:\n\
		stp x19, x20, [x0, 16 * 0]\n\
		stp x21, x22, [x0, 16 * 1]\n\
		stp x23, x24, [x0, 16 * 2]\n\
		stp x25, x26, [x0, 16 * 3]\n\
		stp x27, x28, [x0, 16 * 4]\n\
		stp x29, x30, [x0, 16 * 5]\n\
		mov x9, sp\n\
		str x9, [x0, 16 * 6]\n\
		\n\
		ldp x19, x20, [x1, 16 * 0]\n\
		ldp x21, x22, [x1, 16 * 1]\n\
		ldp x23, x24, [x1, 16 * 2]\n\
		ldp x25, x26, [x1, 16 * 3]\n\
		ldp x27, x28, [x1, 16 * 4]\n\
		ldp x29, x30, [x1, 16 * 5]\n\
		ldr x9, [x1, 16 * 6]\n\
		mov sp, x9\n\
		msr tpidr_el1, x1\n\
		\n\
		ret\n\
	"::);
}

void threadSchedule(){
	if(!rq.beg){
		uart_printf("rq is empty!!\n");
		while(1){}
	}
	
	if(rq.beg==rq.end){
		//no other task, all done
	}else{
		do{
			rq.end->next=rq.beg;
			rq.end=rq.beg;
			rq.beg=rq.beg->next;
			rq.end->next=0;
		}while(rq.beg->status&TASKEXIT);

		//uart_printf("%d %x %x %x\n",rq.beg->id,rq.beg->context[10],rq.beg->context[11],rq.beg->context[12]);
		asm volatile("\
			mov x0, %0\n\
			mov x1, %1\n\
			bl _threadSwitch\n\
		"::"r"(rq.end),"r"(rq.beg));//only use bl to avoid stack usage
	}
}

Task* threadCreate(void* func){
	Task* new_task=(Task*)falloc(TASKSIZE);
	if((unsigned long)new_task%TASKSIZE){//aligned
		uart_printf("new_task isn't aligned!!\n");
		while(1){}
	}
	
	new_task->context[10]=(unsigned long)new_task+TASKSIZE;//fp
	new_task->context[11]=(unsigned long)func;//lr
	new_task->context[12]=(unsigned long)new_task+TASKSIZE;//sp
	new_task->id=task_cnter++;
	new_task->status=0;
	new_task->next=0;

	if(rq.beg){
		rq.end->next=new_task;
		rq.end=rq.end->next;
	}else{
		rq.beg=rq.end=new_task;
	}

	return new_task;
}

void zombiesKill(){//called by idle()
	Task* tar=rq.beg;
	while(1){
		while(tar->next&&(tar->next->status&TASKEXIT)){
			Task* tmp=tar->next->next;
			ffree((unsigned long)(tar->next));
			tar->next=tmp;
		}

		if(!tar->next){
			rq.end=tar;
			break;
		}else{
			tar=tar->next;
		}
	}
}

void exit(){
	Task* cur;
	asm volatile("mrs %0, tpidr_el1\n":"=r"(cur):);
	cur->status|=TASKEXIT;
	threadSchedule();

	while(1){
		uart_printf("exit() failed!!\n");
	}
}

int tidGet(){
	Task* cur;
	asm volatile("mrs %0, tpidr_el1\n":"=r"(cur):);
	return cur->id;
}

void idle(){
	while(1){
		//uart_printf("idle()\n");
		//uart_getc();
		zombiesKill();
		threadSchedule();
	}
}

void foo(){
	for(int i=0;i<10;++i){
		uart_printf("Thread id: %d %d\n",tidGet(),i);
		//uart_getc();
		threadSchedule();
	}

	exit();
}

void threadTest(){
	Task* cur=threadCreate(0);//use kernel stack (not task stack)
	asm volatile("msr tpidr_el1, %0\n"::"r"((unsigned long)cur));

	for(int i=0;i<3;++i){
		threadCreate(foo);
	}

	idle();
}