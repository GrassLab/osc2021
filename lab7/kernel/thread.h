#ifndef THREAD_H
#define THREAD_H

#include "vfs.h"

#define THREAD_SIZE				0x1000
#define TASK_RUNNING			0
#define TASK_DEAD				1
#define TASK_FORK				2

// for lab6 req2
#define FD_MAX 					256

struct fd_table_t
{
	struct file *files[FD_MAX];
};

// kerenl stack context (use for switch_to function in context_swtich.s)
// https://developer.arm.com/documentation/102374/0101/Procedure-Call-Standard
struct cpu_context
{
	// context to kernel-stack (x19 to x28, fp(x29), lr(x30), sp)
	// only use x19 to x28: Callee-saved register
	unsigned long x19;
	unsigned long x20;
	unsigned long x21;
	unsigned long x22;
	unsigned long x23;
	unsigned long x24;
	unsigned long x25;
	unsigned long x26;
	unsigned long x27;
	unsigned long x28;
	unsigned long fp; // x29
	unsigned long lr; // x30
	unsigned long sp; // sp contain x0-x18
	
	// for user program (spsr_el1, elr_el1, sp_el0)(user register x0-x30)
	unsigned long spsr_el1;
	unsigned long elr_el1;
	unsigned long sp_el0;
	unsigned long reg[31];
	
};

// thread struct
struct thread 
{
	struct cpu_context context;
	unsigned long tid;  // thread id
	int status;	
	struct fd_table_t fd_table; // for lab6 req2
	struct thread* next;
	
	unsigned long program_addr;
	unsigned long program_size;
	unsigned long childID;
};

// outer function in context_swtich.s
extern void switch_to(struct thread* prev, struct thread* next);
extern struct thread* get_current();
extern void set_current(struct thread* current);

void init_thread();
void thread_test();
void foo();
struct thread* thread_create(void* func);
void add_to_run_queue(struct thread* new_thread);
struct thread* current_thread();
void schedule();
void kill_zombies();
void idle();
void delay(int count);
void exit();
//
void init_thread2();
void thread_test2();
void user_test();
void exec(const char* name, char(*argv)[10]);
unsigned long generate_user_addr();
void load_program_with_args(const char* name, char(*argv)[10], unsigned long addr, struct thread* current);
unsigned long pass_argument(char(*argv)[10], unsigned long addr);
unsigned long get_pid();
int fork();
void do_fork();
void copy_program(struct thread* parent, struct thread* child);
// for lab6 req2
void thread_vfs_req2();
void vfs_req2();
struct file *thread_get_file(int fd);
int thread_register_fd(struct file *file);
int thread_clear_fd(int fd);
// for lab6 ele1
void thread_vfs_ele1();
void vfs_ele1();
// for lab6 ele2
void thread_vfs_ele2();
void vfs_ele2();
// for lab7 req1
void thread_fat32_req1();
void fat32_req1();
// for lab7 req2
void thread_fat32_req2();
void fat32_req2();
#endif