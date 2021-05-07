#pragma once
#include "utils.h"
#include "system.h"
#include "printf.h"
#include "entry.h"
#include "vfs.h"
#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2
#define FD_MAX 256

typedef struct FD_table
{
    int fd_idx;
    struct file *entry[FD_MAX];
} FD_table;
typedef struct thread_info
{
    unsigned long context[13 + 3 + 31]; // context: (10(reg), fp, lr, sp), (spsr_el1, elr_el1, sp_el0)(user reg)
    int tid;
    struct thread_info *next;
    int status;
    unsigned long p_addr, p_size, child_pid;
    FD_table fd_table;
} thread_info;

typedef struct
{
    unsigned long addr[10];
    int current;
} process_address_generator;
#define THREAD_SIZE 4096
#define THREAD_DEAD 1
#define THREAD_FORK 2

extern void delay(int);
extern uint64_t get_current();
extern void switch_to(unsigned long, unsigned long);

thread_info *Thread(void *);
void add_to_run_queue(thread_info *);
thread_info *current_thread();
unsigned long get_pid();
void schedule();
int fork();
void do_fork();
void copy_program(thread_info *, thread_info *);
void exec(char *, char **);
unsigned long pass_argument(char **, unsigned long);
void load_program_with_args(char *, char **, unsigned long, thread_info *);
unsigned long __generate_user_addr();
void exit();
void foo();
void exec_test();
void idle();
void kill_zombies();
void init_thread();
void thread_test();
void thread_test2();
void exec_ls();
void thread_test3();
int task_register_fd(struct file *entry);
struct file *thread_fd_entry(int fd);