#include "thread.h"
#include "uart.h"
#include "cpio.h"
#include "vfs.h"
#include "error.h"

#define TASKSIZE 4096
#define FD_TABLE_SIZE 5
#define TASKEXIT 1
#define TASKFORK 2

typedef struct _Task{
	unsigned long context[12+1+2+1+31];//kreg+ksp & spsr+elr & usp+ureg
	int id;
	int status;
	unsigned long a_addr,a_size,child;
	file* fd_table[FD_TABLE_SIZE];
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
		}while(rq.beg->status);//ignore abnormal task

		switch_to(get_current() ,rq.beg);
	}
}

Task* threadCreate(void* func){
	Task* new_task=(Task*)alloc_page(TASKSIZE);
	if((unsigned long)new_task%TASKSIZE){//aligned
		uart_printf("new_task isn't aligned!!\n");
		while(1){}
	}
	
	new_task->context[10]=(unsigned long)new_task+TASKSIZE;//fp
	new_task->context[11]=(unsigned long)func;//lr
	new_task->context[12]=(unsigned long)new_task+TASKSIZE;//sp
	new_task->id=task_cnter++;
	new_task->status=0;
	new_task->a_addr=new_task->a_size=new_task->child=0;
	for(int i=0;i<FD_TABLE_SIZE;++i)new_task->fd_table[i]=0;
	new_task->next=0;

	if(rq.beg){
		rq.end->next=new_task;
		rq.end=rq.end->next;
	}else{
		rq.beg=rq.end=new_task;
	}

	return new_task;
}

/*--------------------------------------------*/

void zombiesKill(){//called by idle()
	Task* tar=rq.beg;
	while(1){
		while(tar->next&&(tar->next->status&TASKEXIT)){
			Task* tmp=tar->next->next;
			for(int i=0;i<FD_TABLE_SIZE;++i){
				if(tar->next->fd_table[i]){
					vfs_close(tar->next->fd_table[i]);
				}
			}
			free_page((unsigned long)(tar->next), sizeof(Task));
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

void taskUpdate(Task* p,Task* c){
	p->status^=TASKFORK;
	p->child=c->id;

	Task* tmp=c->next;
	char* src=(char*)p;
	char* dst=(char*)c;
	for(int i=0;i<TASKSIZE;++i){//task copy
		*dst=*src;
		dst++;
		src++;
	}

	c->id=p->child;
	uart_puts("Please enter app load address (Hex): ");
	c->a_addr=uart_getX(1);
	c->child=0;
	for(int i=0;i<FD_TABLE_SIZE;++i)c->fd_table[i]=0;//TODO: give child new fds
	c->next=tmp;

	long k_delta=(long)c-(long)p;
	long a_delta=(long)c->a_addr-(long)p->a_addr;
	c->context[10]+=k_delta;//kernel fp
	c->context[12]+=k_delta;//kernel sp
    c->context[14]+=a_delta;//kernel lr (elr_el1)
    c->context[15]+=a_delta;//user sp (sp_el0)
	c->context[45]+=a_delta;//user fp
	c->context[46]+=a_delta;//user lr

    src=(char*)p->a_addr;
    dst=(char*)c->a_addr;
    for(int i=0; i< p->a_size; i++){//program copy
        *dst=*src;
        dst++;
        src++;
    }
}

void doFork(){//called by idle()
	Task* tar=rq.beg->next;
	while(tar){
		if((tar->status)&TASKFORK){
			Task* child=threadCreate(0);
			taskUpdate(tar,child);
		}
		tar=tar->next;
	}
}

void idle(){
	while(1){
		//uart_printf("idle()\n");
		//uart_getc();
		zombiesKill();
		doFork();
		threadSchedule();
	}
}

/*--------------------------------------------*/

int tidGet(){
	Task* cur;
	asm volatile("mrs %0, tpidr_el1\n":"=r"(cur):);
	return cur->id;
}

void exec(char* path,char** argv){//TODO: reset sp
	unsigned long a_addr;
	uart_puts("Please enter app load address (Hex): ");
	a_addr=uart_getX(1);
	loadApp_with_argv(path,a_addr,argv,&(rq.beg->a_addr),&(rq.beg->a_size));
	exit();
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

int fork(){
	rq.beg->status|=TASKFORK;
	threadSchedule();
	return rq.beg->child;
}

int sys_open(const char *pathname,int flags){
	Task* cur;
	asm volatile("mrs %0, tpidr_el1\n":"=r"(cur):);
	int ret=-1;
	for(int i=0;i<FD_TABLE_SIZE;++i){
		if(cur->fd_table[i]==0){
			ret=i;
			cur->fd_table[i]=vfs_open(pathname,flags);
			break;
		}
	}
	return ret;
}

int sys_close(int fd){
	if(fd<0||fd>=FD_TABLE_SIZE)ERROR("invalid fd!!");
	Task* cur;
	asm volatile("mrs %0, tpidr_el1\n":"=r"(cur):);
	if(cur->fd_table[fd]){
		vfs_close(cur->fd_table[fd]);
		cur->fd_table[fd]=0;
	}
	return 0;
}

int sys_write(int fd,const void *buf,int count){
	if(fd<0||fd>=FD_TABLE_SIZE)ERROR("invalid fd!!");
	Task* cur;
	asm volatile("mrs %0, tpidr_el1\n":"=r"(cur):);
	if(cur->fd_table[fd]){
		return vfs_write(cur->fd_table[fd],buf,count);
	}
	return 0;
}

int sys_read(int fd,void *buf,int count){
	if(fd<0||fd>=FD_TABLE_SIZE)ERROR("invalid fd!!");
	Task* cur;
	asm volatile("mrs %0, tpidr_el1\n":"=r"(cur):);
	if(cur->fd_table[fd]){
		return vfs_read(cur->fd_table[fd],buf,count);
	}
	return 0;
}

/*--------------------------------------------*/

void foo(){
	char buf[10][50];
	char* argv[10];
	for(int i=0;i<10;++i){
		uart_printf("arg%d: ",i);
		int n=uart_gets(buf[i],50,1);
		argv[i]=buf[i];
		if(n==0||i==9){
			argv[i]=0;
			break;
		}
	}
	exec(argv[0],argv);
}

void threadTest(){
	Task* cur=threadCreate(0);//use startup stack (not kernel stack)
	asm volatile("msr tpidr_el1, %0\n"::"r"((unsigned long)cur));

	threadCreate(foo);

	idle();
}

void foo1(){
	for(int i=0;i<10;++i){
		uart_printf("Thread id: %d %d\n",tidGet(),i);
		//uart_getc();
		threadSchedule();
	}

	exit();
}

void threadTest1(){
	Task* cur=threadCreate(0);//use startup stack (not kernel stack)
	asm volatile("msr tpidr_el1, %0\n"::"r"((unsigned long)cur));

	for(int i=0;i<3;++i){
		threadCreate(foo1);
	}

	idle();
}

void foo2(){
	char* argv[]={"argv_test","-o","arg2",0};
	exec("app1",argv);
}

void threadTest2(){
	Task* cur=threadCreate(0);//use startup stack (not kernel stack)
	asm volatile("msr tpidr_el1, %0\n"::"r"((unsigned long)cur));

	threadCreate(foo2);

	idle();
}