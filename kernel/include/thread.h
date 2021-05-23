#pragma once

#include <stdint.h>

#include "vfs.h"

#define STACK_SIZE 4096
#define USER_PROGRAM_BASE 0x30000000
#define USER_PROGRAM_SIZE (1 * mb)

#define THREAD_DEAD 1
#define THREAD_FORK 2
#define THREAD_READY 4

#define STDIN_FILENO 0
#define STDOUT_FILENO 1
#define STDERR_FILENO 2
#define FD_MAX 256

typedef struct {
  uint64_t x19;
  uint64_t x20;
  uint64_t x21;
  uint64_t x22;
  uint64_t x23;
  uint64_t x24;
  uint64_t x25;
  uint64_t x26;
  uint64_t x27;
  uint64_t x28;
  uint64_t fp;
  uint64_t lr;
  uint64_t sp;
} cpu_context;

typedef struct {
  struct file *files[FD_MAX];
} fd_table_t;

typedef struct thread_info {
  cpu_context context;
  uint32_t pid;
  uint32_t child_pid;
  int status;
  uint64_t trap_frame_addr;
  uint64_t kernel_stack_base;
  uint64_t user_stack_base;
  uint64_t user_program_base;
  uint32_t user_program_size;
  fd_table_t fd_table;
  struct thread_info *next;
} thread_info;

typedef struct {
  thread_info *head, *tail;
} thread_queue;

typedef struct {
  uint64_t x[31];
} trap_frame_t;

thread_queue run_queue;
uint32_t thread_cnt;

extern thread_info *get_current();
extern void switch_to(thread_info *, thread_info *);

void foo();
void thread_test1();
void user_test();
void thread_test2();
void thread_vfs_test();
void thread_vfs_ls_test();
void thread_vfs_hard_test();

void thread_init();
thread_info *thread_create(void (*func)());
void schedule();
void idle();
void exit();
void run_queue_push(thread_info *thread);
void kill_zombies();
void exec(const char *program_name, const char **argv);
void fork(uint64_t sp);
void handle_fork();
void create_child(thread_info *parent, thread_info *child);

struct file *thread_get_file(int fd);
int thread_register_fd(struct file *file);
int thread_clear_fd(int fd);
