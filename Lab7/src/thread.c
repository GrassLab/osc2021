#include "thread.h"
#include "uart.h"
#include "mm.h"
#include "cpio.h"
#include "vfs.h"

#define TASKSIZE 4096
#define TASKEXIT 1
#define TASKFORK 2
#define FD_TABLE_SIZE 5
#define BASE 0x20000

int threadnum = 0;
struct Task
{
	//kerenl reg + kernel sp + spsr+elr + user sp + user register
	unsigned long context[12+1+2+1+31];
	int id;

	//fork or exit
	int status;
	unsigned long a_addr, a_size, child;
	file* fd_table[FD_TABLE_SIZE];
	struct Task* next;
};

struct queue_t
{
	struct Task *beg, *end;
};

static struct queue_t queue;
static int task_cnter = 0;

void threadSwithch(){


	/*stp put register in x0
	  ldp pull register from x0
	  put  sp in x9 put x9 in x0
	 */

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
		\n\
		msr tpidr_el1, x1\n\
		\n\
		ret\n\
	"::);
}

void threadSchedule(){

	if(!queue.beg){
		uart_printf("queue is empty\n");
		while(1){}
	}

	if(queue.beg == queue.end){
		// no task can do 
	}
	else{

		// if status equal fork or exit ignore
		do{
			queue.end->next = queue.beg;
			queue.end = queue.beg;
			queue.beg = queue.beg->next;
			queue.end->next = 0;
		}while(queue.beg->status); 

		asm volatile("\
			mov x1, %0\n\
			mrs x0, tpidr_el1\n\
			bl _threadSwitch\n\
		"::"r"(queue.beg));
	}
}

struct Task* threadCreate(void* func){
	struct Task* new_task = (struct Task*)falloc(TASKSIZE);

	if((unsigned long) new_task %TASKSIZE){
		uart_printf("new task no align\n");
		while(1){};
	}

	//frame point
	new_task->context[10] = (unsigned long)new_task + TASKSIZE;
	//linker register
	new_task->context[11] = (unsigned long)func;
	//stack pointer
	new_task->context[12] = (unsigned long)new_task + TASKSIZE;

	new_task->id = task_cnter++;
	new_task->status = 0;
	new_task->a_addr = new_task->a_size = new_task->child = 0;
	for(int i=0;i<FD_TABLE_SIZE;++i)new_task->fd_table[i]=0;
	new_task->next = 0;
	//insert new task at last
	if(queue.beg){
		queue.end->next = new_task;
		queue.end = queue.end->next;
	}
	else{
		queue.beg = queue.end = new_task;
	}

	return new_task;

}

void zombiesKill(){
	struct Task* tar = queue.beg;

	while(1){

		while(tar->next && (tar->next->status & TASKEXIT)){
			struct Task* tmp = tar->next->next;
			for(int i=0;i<FD_TABLE_SIZE;++i){
				if(tar->next->fd_table[i]){
					vfs_close(tar->next->fd_table[i]);
				}
			}
			ffree((unsigned long)(tar->next));
			tar->next = tmp;
			threadnum--;
		}

		//if delete the last one
		if(!tar->next){
			queue.end = tar;
			break;
		}
		else{
			tar = tar->next;
		}
	}
}

void taskUpdate(struct Task* parent, struct Task* child){
	parent->status ^= TASKFORK;
	parent->child = child->id;

	struct Task* tmp = child->next;
	char* src = (char*)parent;
	char* dst = (char*)child;

	for(int i = 0; i < TASKSIZE; ++i){
		*dst = *src;
		dst++;
		src++;
	}

	child->id = parent->child;
	uart_printf("load address: \n");
	threadnum++;
	child->a_addr = uart_getX(1);
	uart_printf("%x \n", child->a_addr);
	child->child = 0;
	for(int i=0;i<FD_TABLE_SIZE;++i)child->fd_table[i]=0;
	child->next = tmp;

	long k_physical = (long)child - (long)parent;
	long a_physical = (long)child->a_addr - (long)parent->a_addr;

	child->context[10] += k_physical; //kernel fp
	child->context[12] += k_physical; //kernel sp
	child->context[14] += a_physical; //erl_el1
	child->context[15] += a_physical; //sp_el0
	child->context[45] += a_physical; //user fp
	child->context[46] += a_physical; //user lr

	src = (char*)(parent->context[15]);
	dst = (char*)(child->context[15]);

	for(int i, ii = parent->a_addr + parent->a_size - parent->context[15]; i < ii; i++){
		*dst = *src;
		dst++;
		src++;
	}

}

void doFork(){
	
	struct Task* tar = queue.beg->next;
	while(tar){
		if((tar->status) & TASKFORK){
			struct Task* child = threadCreate(0);
			
			taskUpdate(tar, child);
		
		}
		tar = tar->next;
	}
	
}

void idle(){
	while(1){
		zombiesKill();
		doFork();
		threadSchedule();
	}
}

int tidGet(){
	struct Task* cur;
	asm volatile("mrs %0, tpidr_el1\n":"=r"(cur):);
	return cur->id;
}
unsigned long putArgv(char** argv,unsigned long ret){
	int cnt1=0,cnt2=0;
	for(int i=0;;++i){
		cnt1++;//with null
		if(!argv[i])break;

		for(int j=0;;++j){
			cnt2++;//with null
			if(!argv[i][j])break;
		}
	}

	int sum=8+8+8*cnt1+cnt2;
	ret=(ret-sum);
	//alignment
	ret=ret-(ret&15);

	char* tmp=(char*)ret;
	*(unsigned long*)tmp=cnt1-1;
	tmp+=8;
	*(unsigned long*)tmp=(unsigned long)(tmp+8);
	tmp+=8;
	char* buffer=tmp+8*cnt1;
	for(int i=0;i<cnt1;++i){
		if(i+1==cnt1){
			*(unsigned long*)tmp=0;
		}else{
			*(unsigned long*)tmp=(unsigned long)buffer;
			tmp+=8;
			for(int j=0;;++j){
				*buffer=argv[i][j];
				buffer++;
				if(!argv[i][j])break;
			}
		}
	}
	return ret;
}
void loadFSApp(char* path,unsigned long a_addr,char** argv,unsigned long* task_a_addr,unsigned long* task_a_size){

	uart_printf("%s\n", path);
	file* f=vfs_open(path,0);

	*task_a_addr=a_addr;
	*task_a_size=vfs_read(f,(void*)a_addr,0x10000);

	vfs_close(f);
	if((*task_a_size)>=0x10000)ERROR("app is too large!");

	uart_puts("loading...\n");
	unsigned long sp_addr=putArgv(argv,a_addr);
	asm volatile("mov x0, 0x340			\n");//enable interrupt
	asm volatile("msr spsr_el1, x0		\n");
	asm volatile("msr elr_el1, %0		\n"::"r"(a_addr));
	asm volatile("msr sp_el0, %0		\n"::"r"(sp_addr));

	asm volatile("mrs x3, sp_el0		\n"::);
	asm volatile("ldr x0, [x3, 0]		\n"::);
	asm volatile("ldr x1, [x3, 8]		\n"::);

	asm volatile("eret					\n");
}
void exec(char* path, char** argv){
	unsigned long a_addr;
	uart_puts("Please enter app load address (Hex): ");
	a_addr=uart_getX(1);
	loadFSApp(path,a_addr,argv,&(queue.beg->a_addr),&(queue.beg->a_size));
	ERROR("exec fail!");
}

void exit(){
	struct Task* cur;
	asm volatile("mrs %0, tpidr_el1\n":"=r"(cur):);
	cur->status |= TASKEXIT;
	threadSchedule();
	

}

int fork(){
	queue.beg->status |= TASKFORK;
	threadSchedule();
	uart_printf("fork return %d\n", queue.beg->child);
	return queue.beg->child;
}

int sys_open(const char *pathname,int flags){
	struct Task* cur;
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
	
	struct Task* cur;
	asm volatile("mrs %0, tpidr_el1\n":"=r"(cur):);
	if(cur->fd_table[fd]){
		vfs_sync(cur->fd_table[fd]);
		vfs_close(cur->fd_table[fd]);
		cur->fd_table[fd]=0;
	}
	return 0;
}

int sys_write(int fd,const void *buf,int count){
	
	struct Task* cur;
	asm volatile("mrs %0, tpidr_el1\n":"=r"(cur):);
	if(cur->fd_table[fd]){
		return vfs_write(cur->fd_table[fd],buf,count);
	}
	return 0;
}

int sys_read(int fd,void *buf,int count){
	
	struct Task* cur;
	asm volatile("mrs %0, tpidr_el1\n":"=r"(cur):);
	if(cur->fd_table[fd]){
		return vfs_read(cur->fd_table[fd],buf,count);
	}
	return 0;
}

void test1(){
	for(int i = 0; i < 10; i++){
		int id = tidGet();
		uart_printf("Thread id: %d %d\n", id, i);
		threadSchedule();
	}
	exit();
}

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
	struct Task* cur=threadCreate(0);//use startup stack (not kernel stack)
	asm volatile("msr tpidr_el1, %0\n"::"r"((unsigned long)cur));

	threadCreate(foo);

	idle();
	ffree((unsigned long)cur);
}

void threadtest1(){
	
	struct Task* cur = threadCreate(0);
	uart_printf("start test 1\n");
	asm volatile("msr tpidr_el1, %0\n"::"r"((unsigned long) cur));
	uart_printf("start test 1\n");
	for(int i = 0; i < 3; i++){
		uart_printf("start test 1\n");
		threadCreate(test1);
	}

	idle();
}

void test2(){
	char* argv[] = {"argv_test", "arg2", "0"};

	exec("app1", argv);
}

void threadtest2(){
	struct Task* cur = threadCreate(0);
	asm volatile("msr tpidr_el1, %0\n"::"r"((unsigned long)cur));
	threadCreate(test2);
	idle();
}
