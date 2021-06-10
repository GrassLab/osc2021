#include "../include/uart.h"
#include "../include/task.h"
#include "../include/memAlloc.h"
#include "../include/initrd.h"
#include "../include/shell.h"
#include "../include/switch.h"
#include "../include/list.h"
#include "../include/interrupt.h"
#include "../include/stringUtils.h"
#include "../include/vfs.h"
#include "../include/tmpfs.h"
#include "../include/gpio.h"

#define BUF_MAX_SIZE 128
#define TASKSIZE 4096
#define TASK_ALIVE 1
#define TASK_DEAD 2

static RUN_Q run_q;
static RUN_Q exit_q;
static int tidd = 0;
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
unsigned long uart_getX(int display){
	unsigned long ret=0;
	char c;
	while(1){
		c=uart_getc();
		if(display)uart_send(c);
		if(c=='\n')break;
		if(c>='0'&&c<='9'){
			ret=ret*16+c-'0';
		}else if(c>='a'&&c<='f'){
			ret=ret*16+c-'a'+10;
		}else if(c>='A'&&c<='F'){
			ret=ret*16+c-'A'+10;
		}
	}
	return ret;
}

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
    for(int i = 0 ;i<5;++i){
        new_task->fd_table[i] = 0;
    }
    new_task->stack_addr = 0;
    new_task->page_table = 0;
    new_task->a_addr= 0;

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
    //change page table
		asm volatile("\
			mov x0, %0\n\
			dsb ish\n\
			msr ttbr0_el1, x0\n\
			tlbi vmalle1is\n\
			dsb ish\n\
			isb\n\
		"::"r"(next_node->task->page_table));
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
    for(int i = 0;i<5;++i){
        if(cur->fd_table[i]){
            sys_close(i);
        }
    }
 	if(cur->page_table)removePT(cur->page_table,0);
    threadSchedule();
}

void idle(){
    //uart_printf("in idle\n");
    while(1){
        zombiekill();
        threadSchedule();
    }
}
//void exec(char *path, char** argv){
//    task_struct *cur = get_current();
//
//    unsigned long a_addr;
//    char buf[BUF_MAX_SIZE];
//    uart_puts("please enter app load address:\n");
//    read_input(buf);
//    a_addr = getHexFromString(buf);
//    cur->stack_addr = a_addr;
//    unsigned long sp_addr;
//    sp_addr = loadprogWithArgv(path, a_addr, argv);
//    uart_printf("loadsuce\n");
//}
void loadFSApp(char* path,unsigned long a_addr,char** argv,task_struct* task){
	task->a_addr=a_addr;
	//task->a_size=?;//TODO

	//init page table
	initPT(&(task->page_table));
    //uart_printf("here\n");
	asm volatile("mov x0, %0 			\n"::"r"(task->page_table));
	asm volatile("msr ttbr0_el1, x0 	\n");

	//load program
	file* f=vfsOpen(path,0);
	for(unsigned long va=a_addr;;va+=4096){
		updatePT(task->page_table,(void*)va);
		int cnt=vfsRead(f,(void*)va,4096);
		if(cnt<4096)break;
	}
	vfsClose(f);

	//load arg
	updatePT(task->page_table,(void*)(a_addr-4096));
	unsigned long sp_addr=putArgv(argv,a_addr);

	uart_puts("loading...\n");
	asm volatile("mov x0, 0x340			\n");//enable interrupt
	asm volatile("msr spsr_el1, x0		\n");
	asm volatile("msr elr_el1, %0		\n"::"r"(a_addr));
	asm volatile("msr sp_el0, %0		\n"::"r"(sp_addr));

    //core_timer_enable();
	asm volatile("mov x3, %0			\n"::"r"(sp_addr));
	asm volatile("ldr x0, [x3, 0]		\n"::);
	asm volatile("ldr x1, [x3, 8]		\n"::);

	asm volatile("eret					\n");
}
int gettid(){

    task_struct *cur = get_current();
    return cur->id;

}
void exec(char* path,char** argv){//TODO: reset sp
    task_struct *cur = get_current();
	unsigned long a_addr;
	while(1){
		uart_puts("Please enter app link address (Hex): ");
		a_addr=uart_getX(1);
		if(a_addr%4096){
			uart_puts("Not aligned!\n");
		}else{
			break;
		}
	}
    //cur->stack_addr = a_addr;
	loadFSApp(path,a_addr,argv,cur);
	ERROR("exec fail!");
}

//void spppp(unsigned long *sp) {
//    uart_printf("sp is at: %x\n", sp);
//    for(int i = 0 ;i< 35 ; ++i){
//        uart_printf("%x: %x\n", i, sp[i]);
//        //uart_printf("%x: %x\n\n", i, new_context[i]);
//    }
//    // char buf[8];
//    // read_input(buf);
//}
//
//void stop(void) {
//    char buf[8];
//    task_struct *cur = get_current();
//    uart_printf("forked lr: %x\n", cur->context.lr);
//    // unsigned long *cur_context = cur->context;
//    // for(int i = 0 ;i< 35 ; ++i){
//    //     uart_printf("%x: %x\n", i, cur_context[i]);
//    //     //uart_printf("%x: %x\n\n", i, new_context[i]);
//    // }
//    uart_puts("put tido retrun\n");
//    // read_input(buf);
//
//    _child_return_from_fork();
//}
void sys_fork(trap_frame *tf){
    task_struct *parent = get_current();
    task_struct *child = threadCreate(0);

    //uart_printf("%x\n",parent);
    //uart_printf("%x\n",child);
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
    initPT(&(child->page_table));
	dupPT(parent->page_table,child->page_table,0);

    parent->context.sp =(unsigned long)tf;
    int parent_ustack_size;
    if(child_id == 2){
        parent_ustack_size = (parent->stack_addr)-(tf->sp_el0);
    }else if(child_id == 3){
        parent_ustack_size = (parent->stack_addr)-(tf->sp_el0);
    }
    //uart_printf("parent_ustack_addr:%x\n", parent -> stack_addr);
    //uart_printf("parent_sp_el0:%x\n", tf -> sp_el0);
    //uart_printf("parent ustack size:%x\n", parent_ustack_size);
    if ((unsigned long)child > (unsigned long)parent) {
        child->context.sp = parent->context.sp + ((unsigned long)child - (unsigned long)parent);
    } else {
        child->context.sp = parent->context.sp - ((unsigned long)parent - (unsigned long)child);
    }
   // uart_printf("child sp: %x\n", child->context.sp);
    child->context.fp =(unsigned long)(child+TASKSIZE);
    //uart_printf("return from%x\n",_child_return_from_fork);
    child->context.lr =(unsigned long)(_child_return_from_fork);
    child->id = child_id;
    trap_frame *child_tf =(trap_frame*)(child->context.sp);
    //uart_puts("Please enter child ustack address\n");
    //char buf[128];
    //read_input(buf);
    //unsigned long child_ustack = getHexFromString(buf);

    unsigned long child_ustack = (unsigned long)(my_alloc(4096) + 4096);

    //char *src_stack = (char*)(tf->sp_el0);
    //char *dst_stack = (char*)(child_ustack - parent_ustack_size);
    //child -> stack_addr = child_ustack;

    //child_tf->sp_el0 = child_ustack - parent_ustack_size;
    //uart_printf("child sp_el0: %x\n",child_tf->sp_el0);
    child_tf->regs[0] = 0;
    //child_tf->regs[29] = child_ustack;
    tf->regs[0] = child->id;
    //uart_printf("%x\n",tf->sp_el0);
    //uart_printf("%x\n",parent_ustack);
    //while(parent_ustack_size--){
    //    *dst_stack = *src_stack;
    //    src_stack++;
    //    dst_stack++;
    //}
    //uart_printf("fork done\n"); 
    return;

}


int getpid(trap_frame* tf){
    task_struct *cur = get_current();
    tf->regs[0] = cur->id;
}

int sys_open(char* pathname, int flags){
    task_struct *cur = get_current();
    file* file = vfsOpen(pathname,flags);
    int ret = -1;

    for(int i = 0;i<5;  ++i){
        if(cur->fd_table[i]==0){
            cur->fd_table[i] = file;
            ret = i;
            break;
        }
    }
    return ret;
}

int sys_close(int file_index){
    task_struct *cur = get_current();
    if(cur->fd_table[file_index]){
        vfsSync(cur->fd_table[file_index]);
        vfsClose(cur->fd_table[file_index]);
        cur->fd_table[file_index] = 0;
    }
    return 0;
}

int sys_write(int file_index, char* input, int length){
    task_struct *cur = get_current();
    file* task_file = cur->fd_table[file_index];
    return vfsWrite(task_file,input,length);
}

int sys_read(int file_index, char* output, int length){
    task_struct *cur = get_current();
    file* task_file = cur->fd_table[file_index];
    return vfsRead(task_file,output,length);
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
   // exec("test.img", argv);
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
void foo3(){
    char* argv[] = {"HIHI.TXT","HIHI.TXT",0};
    exec("APP5",argv);
    cur_exit();
    return ;


}

void foo4(){
    char* argv[] = {"HIHI.TXT","HIHI.TXT",0};
    exec("APP4",argv);
    cur_exit();
    return ;

}

void test3(){
    tidd = 0;
    run_q.beg = run_q.end = 0;
    exit_q.beg = exit_q.end = 0;
    task_struct* root_task = threadCreate(idle);
    asm volatile("msr tpidr_el1, %0\n" ::"r"(root_task));
    threadCreate(foo3);
    idle();


}

void test4(){
    tidd = 0;
    run_q.beg = run_q.end = 0;
    exit_q.beg = exit_q.end = 0;
    task_struct* root_task = threadCreate(idle);
    asm volatile("msr tpidr_el1, %0\n" ::"r"(root_task));
    threadCreate(foo4);
    idle();

}
