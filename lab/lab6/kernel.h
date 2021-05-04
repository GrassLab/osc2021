#include "bootloader.h"
#include "printf.h"
#include "uart.h"
#include "utils.h"

void shell();
void pause() {
  printf("Press any key to continue . . .");
  uart_getc();
  printf("\r                                \r");
}
/* cpio */
#define CPIO_ARRD 0x8000000
#define CPIO_MAGIC_BYTES 6
#define CPIO_OTHERS_BYTES 8
#define CPIO_SIZE (CPIO_MAGIC_BYTES + CPIO_OTHERS_BYTES * 13)

typedef struct cpio_newc_header {
  char c_magic[6];
  char c_ino[8];
  char c_mode[8];
  char c_uid[8];
  char c_gid[8];
  char c_nlink[8];
  char c_mtime[8];
  char c_filesize[8];
  char c_devmajor[8];
  char c_devminor[8];
  char c_rdevmajor[8];
  char c_rdevminor[8];
  char c_namesize[8];
  char c_check[8];
} cpio_newc_header;

int cpio_info(cpio_newc_header **cpio_ptr, char **cpio_addr, char **context) {
  *cpio_ptr = (cpio_newc_header *)(*cpio_addr);

  /* get filename size */
  unsigned long long int c_namesize =
      atoHex_size((*cpio_ptr)->c_namesize, CPIO_OTHERS_BYTES);

  /* get cpio header size */
  unsigned long long int header_size =
      CPIO_SIZE + c_namesize + align(CPIO_SIZE + c_namesize, 4);

  /* get cpio context size */
  unsigned long long int context_size =
      atoHex_size((*cpio_ptr)->c_filesize, CPIO_OTHERS_BYTES);
  unsigned long long int size = context_size + align(context_size, 4);
  *context = (*cpio_addr) + header_size;

  /* move to next cpio header */
  *cpio_addr += header_size + size;
  return context_size;
}

/* buddy system */
#define BUDDY_START 0x10000000
#define BUDDY_END 0x20000000
#define PAGE_SIZE (4 * KB)
#define BUDDY_ARRAY_SIZE ((BUDDY_END - BUDDY_START) / PAGE_SIZE)
#define BUDDY_INDEX 20

typedef struct buddy_list {
  struct buddy_list *next;
  void *addr;
  int size;
} buddy_list;
buddy_list buddy[BUDDY_ARRAY_SIZE];
buddy_list *free_list[BUDDY_INDEX];
buddy_list *used_list[BUDDY_INDEX];

void buddy_init(void *mem_start) {
  for (int i = 0; i < BUDDY_INDEX; i++) free_list[i] = used_list[i] = 0;
  for (int i = 0; i < BUDDY_ARRAY_SIZE; i++)
    buddy[i] =
        (buddy_list){.next = 0, .addr = mem_start + i * PAGE_SIZE, .size = -1};
  int begin_size = 2 << (BUDDY_INDEX - 1);
  for (int size = begin_size, i = 0; size > 0; size >>= 1)
    for (; i + size <= BUDDY_ARRAY_SIZE; i += size) {
      if (i + 2 * size <= BUDDY_ARRAY_SIZE) buddy[i].next = &buddy[i + size];
      buddy[i].size = size;
    }
  for (int size = begin_size, reminder_size = BUDDY_ARRAY_SIZE, i = 0; size > 0;
       size >>= 1)
    if (buddy[i].size == size) {
      free_list[size2Index(size)] = &buddy[i];
      i += (reminder_size / size) * size;
      reminder_size %= size;
    }
}
buddy_list *buddy_alloc(int size) {
  size /= PAGE_SIZE;
  if (free_list[size2Index(size)] == 0) {
    buddy_list *now = buddy_alloc(2 * size * PAGE_SIZE);
    used_list[size2Index(2 * size)] = now->next;
    free_list[size2Index(size)] = now;
    now->size = size;
    now->next = now + now->size;
    now = now->next;
    now->size = size;
    now->next = 0;
  }
  buddy_list *now = free_list[size2Index(size)];
  free_list[size2Index(size)] = now->next;
  now->next = used_list[size2Index(size)];
  used_list[size2Index(size)] = now;
  return now;
}
buddy_list *__buddy_merge__(buddy_list *now, buddy_list *next,
                            buddy_list **before) {
  int size = now->size;
  if (now > next) swap((void *)&now, (void *)&next);
  if (now + size != next ||
      ((uint64_t)now->addr - BUDDY_START) / PAGE_SIZE % (size << 1) != 0)
    return 0;
  *before = next->next;
  next->size = -1;
  next->next = 0;
  now->size <<= 1;
  now->next = 0;
  return now;
}
void buddy_free(buddy_list *list) {
  int size = list->size;
  buddy_list *now = used_list[size2Index(size)],
             **before = &used_list[size2Index(size)];
  for (; now; before = &now->next, now = now->next)
    if (now == list) break;
  *before = list->next;
  now = free_list[size2Index(size)];
  before = &free_list[size2Index(size)];
  buddy_list *merge;
  for (; now; before = &now->next, now = now->next) {
    if ((merge = __buddy_merge__(list, now, before))) {
      buddy_free(merge);
      return;
    }
  }

  list->next = free_list[size2Index(size)];
  free_list[size2Index(size)] = list;
}

void print_buddyList(buddy_list **lists) {
  for (int i = 0; i < BUDDY_INDEX; i++) {
    if (lists[i] != 0) {
      printf("index: %d", i);
      for (buddy_list *now = lists[i]; now != 0; now = now->next)
        printf("    memory addr: 0x%010x\n", now->addr);
    }
  }
}
/*
  dma
*/
typedef struct dma {
  struct dma *next;
  buddy_list *page;
  int size;
} dma;
dma *free_pool, *used_pool;
void dma_init() { free_pool = used_pool = 0; }

void *kmalloc(int size) {
  size += align(size, 8);
  int dmaSize = sizeof(dma) + align(sizeof(dma), 8);
  /* get appropriate size*/
  int min = GB;
  dma *result = 0, **rBefore = &free_pool;
  for (dma *now = free_pool, **before = &free_pool; now;
       before = &now->next, now = now->next) {
    int delta = now->size - dmaSize - size;
    if (0 < delta && delta < min) {
      min = delta;
      result = now;
      rBefore = before;
    }
  }
  if (result == 0) {
    int buddy_size = (2 * dmaSize + size) / PAGE_SIZE;
    buddy_size =
        PAGE_SIZE << ((buddy_size == 0) ? 0 : (size2Index(buddy_size) + 1));
    buddy_list *page = buddy_alloc(buddy_size);
    result = (dma *)page->addr;
    result->size = buddy_size - dmaSize;
    result->page = page;
    result->next = free_pool;
  };
  dma *list = (dma *)((void *)result + dmaSize + size);
  list->next = result->next;
  list->page = result->page;
  list->size = result->size - dmaSize - size;
  *rBefore = list;

  /* alloc */
  result->next = used_pool;
  result->size = size;
  used_pool = result;
  return (void *)result + dmaSize;
}

void *__dma_merge__(dma *now, dma *next, dma **before) {
  if (now > next) swap((void *)&now, (void *)&next);
  int size = now->size, dmaSize = sizeof(dma) + align(sizeof(dma), 8);
  if ((void *)now + size + dmaSize != (void *)next || now->page != next->page)
    return 0;
  *before = (*before)->next;
  now->size += next->size + dmaSize;
  now->next = 0;
  return (void *)now + dmaSize;
}

void kfree(void *addr) {
  int dmaSize = sizeof(dma) + align(sizeof(dma), 8);
  dma *list = (dma *)(addr - dmaSize), *now = used_pool, **before = &used_pool;
  for (; now; before = &now->next, now = now->next)
    if (now == list) break;
  // if (now == NULL) {
  //   printf("kfree error: 0x%08x\n", addr);
  //   return;
  // }
  *before = list->next;
  now = free_pool;
  before = &free_pool;
  void *merge;
  for (; now; before = &now->next, now = now->next)
    if ((merge = __dma_merge__(list, now, before))) {
      kfree(merge);
      return;
    }
  if (list->size + dmaSize == list->page->size * PAGE_SIZE) {
    buddy_free(list->page);
    return;
  }
  list->next = free_pool;
  free_pool = list;
}
void printDmaPool(dma *list) {
  for (dma *now = list; now != 0; now = now->next)
    printf("addr: 0x%010x\tsize: 0x%010x\n", ((void *)now) + sizeof(dma),
           now->size);
  printf("pool end\n");
}
/*
  timer &  time's functions
*/
#define CORE0_TIMER_IRQ_CTRL 0x40000040
extern void _timer_enable();
extern void _timer_disable();
extern void _timer_add(uint64_t);
extern uint64_t _get_freq();
typedef struct timer_list {
  uint64_t after;
  struct timer_list *next;
  void *return_addr;
} timer_list;
timer_list *timer_head;
uint64_t current, cntfrq_el0;
uint64_t _update_current() {
  uint64_t new_current, delta;
  asm volatile("mrs %0, cntpct_el0" : "=r"(new_current));
  delta = new_current - current;
  current = new_current;
  return delta;
}
void timer_init() {
  timer_head = 0;
  current = 0;
  cntfrq_el0 = _get_freq();
  _update_current();
  _timer_add((uint64_t)10000 * cntfrq_el0);
  _timer_enable();
}
void _update_afters() {
  uint64_t delta = _update_current();
  for (timer_list *now = timer_head; now; now = now->next) now->after -= delta;
}
void add_timer(void *return_addr, uint64_t after) {
  _update_afters();
  after *= cntfrq_el0;
  /* insert to queue */
  for (timer_list *now = timer_head; now; now = now->next)
    if (now->after < after && (after < now->next->after || now->next == 0)) {
      timer_list *ptr = kmalloc(sizeof(timer_list));
      *ptr = (timer_list){
          .next = now->next, .return_addr = return_addr, .after = after};
      now->next = ptr;
    }
  if (timer_head == 0) {
    timer_list *ptr = kmalloc(sizeof(timer_list));
    *ptr = (timer_list){.next = 0, .return_addr = return_addr, .after = after};
    timer_head = ptr;
  } else if (after < timer_head->after) {
    timer_list *ptr = kmalloc(sizeof(timer_list));
    *ptr = (timer_list){
        .next = timer_head, .return_addr = return_addr, .after = after};
    timer_head = ptr;
  }
  _timer_add(timer_head->after);
}
void _timer_handler() {
  uart_puts("timer handler\r\n");
  if (timer_head->return_addr != 0)
    asm volatile("msr ELR_EL1, %0;" ::"r"(timer_head->return_addr));
  timer_list *ptr = timer_head;
  timer_head = timer_head->next;
  kfree((void *)ptr);
  if (timer_head != 0) {
    _timer_enable();
    _update_afters();
    _timer_add(timer_head->after);
  }
}
/* thread and process */
#define ACTIVE 0
#define ALIVE 1
#define WAIT 2
#define DEAD 3
#define FORK 4
#define STACK_SIZE (PAGE_SIZE - sizeof(dma))

void exit();
struct thread_list;
typedef struct process_list {
  struct process_list *prev, *next;
  struct thread_list *thread;
  void *context_malloc;
  uint64_t pid, context_size, context_count;
} process_list;

void print_process_q(process_list *);
typedef struct thread_list {
  struct thread_list *next, *prev;
  process_list *process;
  void *el0_stack_malloc, *el1_stack_malloc, *sp_el0, *fp_el0;
  void *regs[10], *fp, *lr, *sp;
  uint64_t status, tid;
} thread_list;

thread_list *run_q, kernel_shell;

extern void switch_to(void *prev, void *next);
extern void *get_current();
void idle();
void schedule2(thread_list *);

void print_thread_q(thread_list *q) {
  if (q == 0) return;
  for (thread_list *now = q;; now = now->next) {
    printf("tid: %d\tstatus: %d,\n", now->tid, now->status);
    if (now->next == q) break;
  }
  return;
}

int thread_list_len(thread_list *q) {
  if (q == 0) return 0;
  int i = 1;
  for (thread_list *now = q; now->next != q; now = now->next) ++i;
  return i;
}

thread_list *find_tid(thread_list *q, uint64_t tid) {
  if (q == 0) return 0;
  for (thread_list *now = q;; now = now->next) {
    if (tid == now->tid) return now;
    if (now->next == q) break;
  }
  return 0;
}
thread_list *find_thread(thread_list *q, thread_list *thread) {
  if (q == 0) return 0;
  for (thread_list *now = q;; now = now->next) {
    if (thread == now) return now;
    if (now->next == q) break;
  }
  return 0;
}
thread_list *get_current_thread() { return run_q; }
uint64_t gettid() { return run_q->tid; }
uint64_t get_free_tid() {
  for (uint64_t tid = 0;; ++tid)
    if (find_tid(run_q, tid) == 0) return tid;
}

void add_to_thread_q(thread_list **q, thread_list *item) {
  item->next = item->prev = item;
  if (*q) {
    item->next = (*q);
    item->prev = (*q)->prev;
    (*q)->prev->next = item;
    (*q)->prev = item;
  } else
    *q = item;  // here may change run_q
}
void del_thread(thread_list **q, thread_list *item) {
  /* caller have to make sure iten is in q */
  if (*q == item) *q = item->next;  // here may change run_q
  if (*q == (*q)->next) *q = 0;     // here may change run_q
  item->prev->next = item->next;
  item->next->prev = item->prev;
  // item->prev = item->next = item;
}

void pop_thread(thread_list **q) {
  del_thread(q, *q);  // here may change run_q
}
void thread_delete(thread_list *item) {
  /* caller have to make sure it is not in any queue */
  kfree(item->sp);
  kfree(item);
}
thread_list *thread_create(void (*func_ptr)()) {
  thread_list *new = kmalloc(sizeof(thread_list));
  void *el1_stack_malloc = kmalloc(STACK_SIZE);
  *new = (thread_list){.next = new,
                       .prev = new,
                       .process = 0,
                       .el0_stack_malloc = 0,
                       .el1_stack_malloc = el1_stack_malloc,
                       .sp_el0 = 0,
                       .fp_el0 = 0,
                       .regs = {},
                       .fp = el1_stack_malloc + STACK_SIZE,
                       .lr = func_ptr,
                       .sp = el1_stack_malloc + STACK_SIZE,
                       .status = ALIVE,
                       .tid = get_free_tid()};
  add_to_thread_q(&run_q, new);
  return new;
}
thread_list *thread_copy(thread_list *original) {
  thread_list *new = kmalloc(sizeof(thread_list));
  void *el0_stack_malloc = 0;
  void *el1_stack_malloc = kmalloc(STACK_SIZE);
  if (original->el0_stack_malloc) el0_stack_malloc = kmalloc(STACK_SIZE);
  uint64_t delta_fp = original->fp - original->el1_stack_malloc,
           delta_sp = original->sp - original->el1_stack_malloc,
           delta_sp_el0 = original->sp_el0 - original->el0_stack_malloc,
           delta_fp_el0 = original->fp_el0 - original->el0_stack_malloc;
  *new = (thread_list){.next = new,
                       .prev = new,
                       .el0_stack_malloc = el0_stack_malloc,
                       .el1_stack_malloc = el1_stack_malloc,
                       .sp_el0 = delta_sp_el0 + el0_stack_malloc,
                       .fp_el0 = delta_fp_el0 + el0_stack_malloc,
                       .fp = delta_fp + el1_stack_malloc,
                       .lr = original->lr,
                       .sp = delta_sp + el1_stack_malloc,
                       .status = ALIVE,
                       .tid = get_free_tid()};
  if (el0_stack_malloc)
    memcpy(new->el0_stack_malloc, original->el0_stack_malloc, STACK_SIZE);
  memcpy(new->el1_stack_malloc, original->el1_stack_malloc, STACK_SIZE);
  memcpy(new->regs, original->regs, sizeof(original->regs));
  add_to_thread_q(&run_q, new);
  return new;
}

void thread_init() {
  run_q = 0;
  thread_create(idle);
}

void schedule2(thread_list *to) {
  /*
   * 1. should make sure to->state is ALIVE by caller
   * 2. current thread may ACTIVE or DEAD
   */
  delay(1000);
  if (find_thread(run_q, to) == 0) return;
  if (run_q->status == ACTIVE) run_q->status = ALIVE;
  run_q = to;
  run_q->status = ACTIVE;

  void *tpdir_el1 = get_current();

  /* Here is make sure if first run idle from kernel shell */
  if (tpdir_el1 > (void *)BUDDY_END || tpdir_el1 < (void *)BUDDY_START)
    tpdir_el1 = kernel_shell.regs;

  switch_to(tpdir_el1, run_q->regs);
}

void schedule() {
  /* 1. first call schedule at least has a thread idel
   * 2. schedule just run all ALIVE thread ONE time each one call
   */

  /* if queue only have idle switch to kernel shell */
  if (run_q == run_q->next) {
    switch_to(get_current(), kernel_shell.regs);
  }
  /* round robin */
  for (thread_list *next = run_q->next;; next = next->next) {
    if (next->status == ALIVE) schedule2(next);
    if (next->next == run_q) break;
  }
}

void exit_thread() {
  run_q->status = DEAD;
  schedule();
}

void kill_zombies() {
  /* this is just call by idle so here at least exist one thread "idle" */
  if (run_q == 0) return;
  for (thread_list *now = run_q->next; run_q && run_q->next != run_q;
       now = now->next) {
    if (now->status == DEAD) {
      del_thread(&run_q, now);
    }
    if (now->next == run_q) break;
  }
}

/*
 *   fork & exec
 */

process_list *process_head;
int getpid() {
  if (run_q->process) return run_q->process->pid;
  return -1;
}

void print_process_q(process_list *q) {
  if (q == 0) return;
  for (process_list *now = q;; now = now->next) {
    printf("pid: %d, tid: %d, status: %d\n", now->pid, now->thread->tid,
           now->thread->status);
    if (now->next == q) break;
  }
  return;
}

process_list *find_pid(process_list *q, uint64_t pid) {
  if (q == 0) return 0;
  for (process_list *now = q;; now = now->next) {
    if (pid == now->pid) return now;
    if (now->next == q) break;
  }
  return 0;
}

uint64_t get_free_pid() {
  for (uint64_t pid = 1;; pid++)
    if (find_pid(process_head, pid) == 0) return pid;
  return 0;
}

void add_to_process_q(process_list **q, process_list *item) {
  item->next = item->prev = item;
  if (*q) {
    item->next = (*q);
    item->prev = (*q)->prev;
    (*q)->prev->next = item;
    (*q)->prev = item;
  } else
    *q = item;
}

void del_process(process_list **q, process_list *item) {
  if (*q == item) *q = item->next;
  if (*q == (*q)->next) *q = 0;
  item->prev->next = item->next;
  item->next->prev = item->prev;
  item->prev = item->next = item;
}

process_list *process_create(thread_list *thread, void *context_malloc,
                             uint64_t context_size) {
  process_list *new = kmalloc(sizeof(process_list));
  *new = (process_list){.prev = new,
                        .next = new,
                        .context_malloc = context_malloc,
                        .thread = thread,
                        .pid = get_free_pid(),
                        .context_size = context_size,
                        .context_count = 1};
  thread->process = new;
  add_to_process_q(&process_head, new);
  return new;
}

void exit_process() {
  del_process(&process_head, run_q->process);
  kfree(run_q->process);
  /* will not free process context_malloc,
   * although it have to be freed at some condition
   */
}

void process_init() { process_head = 0; }
void process_copy() {
  if (run_q == 0) return;
  for (thread_list *now = run_q;; now = now->next) {
    if (now->status == FORK) {
      process_create(thread_copy(now), now->process->context_malloc,
                     now->process->context_size);
      now->status = ALIVE;
    }
    if (now->next == run_q) break;
  }
}
int fork() {
  run_q->status = FORK;
  thread_list *parent = run_q;
  schedule();
  if (run_q == parent) return run_q->process->pid;
  return 0;
}

int exec(char *filename, char *argv[]) {
  char *now_addr = (char *)CPIO_ARRD, *name, *context;
  struct cpio_newc_header *cpio_header;
  unsigned long long int context_size = 0;
  do {
    name = now_addr + CPIO_SIZE;
    context_size = cpio_info(&cpio_header, &now_addr, &context);
  } while (!strcmp("TRAILER!!!", name) && !strcmp(filename, name));
  /* check file exist */
  if (!strcmp(filename, name)) return -1;

  /* get cpio context size */
  char *context_malloc = (void *)0x20000000;
  for (int i = 0; i < context_size; ++i) context_malloc[i] = context[i];
  run_q->process->context_size = context_size;
  run_q->process->context_malloc = context_malloc;

  void *el0_stack_malloc = kmalloc(STACK_SIZE);
  char *user_sp = el0_stack_malloc + STACK_SIZE;
  run_q->el0_stack_malloc = el0_stack_malloc;

  uint64_t argc = 0;
  while (argv[++argc] != NULL)  // count argc
    ;

  int total_size =
      sizeof(uint64_t) + sizeof(char **) + sizeof(char *) * (argc + 1);
  for (int i = 0; i < argc; ++i)
    total_size += sizeof(char) * (strlen(argv[i]) + 1);
  user_sp -= align(total_size, 16);

  /* copy *argv[] */
  for (int i = argc - 1; i >= 0; --i) {
    int size = sizeof(char) * (strlen(argv[i]) + 1);
    user_sp -= size;
    memcpy(user_sp, argv[i], size);
    argv[i] = (char *)user_sp;
  }
  argv[argc] = NULL;

  /* copy argv[] */
  for (int i = argc; i >= 0; --i) {
    int size = sizeof(char *);
    user_sp -= size;
    memcpy(user_sp, &argv[i], size);
  }

  /* copy argv */
  argv = (char **)user_sp;
  user_sp -= sizeof(char **);
  memcpy(user_sp, &argv, sizeof(char **));

  /* copy argc*/
  user_sp -= sizeof(uint64_t);
  memcpy(user_sp, &argc, sizeof(uint64_t));

  _run_el0(run_q->process->context_malloc, user_sp);
  return 0;
}

void exit() {
  if (run_q->process) exit_process();
  exit_thread();
}

void idle() {
  while (1) {
    kill_zombies();
    process_copy();
    schedule();
    if (run_q == run_q->next) exit();
  }
}

#define EL0_SYNC_EXIT 0
#define EL0_SYNC_UART_SEND 1
#define EL0_SYNC_FORK 2
#define EL0_SYNC_PID 3
#define EL0_SYNC_EXEC 4
#define EL0_SYNC_UART_GETC 5
#define EL0_SYNC_UART_PUTS 6

#define EL0_SYNC_TEST 20
#define EL0_SYNC_PRINT_H 21

int _el0_sync_entry() {
  uint64_t *entry_sp;
  asm volatile("mov %0, x0" : "=r"(entry_sp));

  uint64_t x0, x1;
  asm volatile("ldr %0, [%1, 0]\n" : "=r"(x0) : "r"(entry_sp));
  asm volatile("ldr %0, [%1, 8]\n" : "=r"(x1) : "r"(entry_sp));

  /* update current thread sp_el0 & fp_el0 */
  asm volatile("ldr %0, [%1, 16 * 15 + 8]\n"
               : "=r"(run_q->sp_el0)
               : "r"(entry_sp));
  asm volatile("ldr %0, [%1, 16 * 14 + 8]\n"
               : "=r"(run_q->fp_el0)
               : "r"(entry_sp));
  uint64_t esr, result = 0;
  asm volatile("mrs %0, esr_el1" : "=r"(esr) :);
  if (((esr >> 26) & 0x3f) == 0x15) {
    uint64_t svc = esr & 0x1ffffff;
    if (svc == EL0_SYNC_EXIT)
      exit();
    else if (svc == EL0_SYNC_UART_SEND)
      printf("%c", (char)x0);
    else if (svc == EL0_SYNC_UART_PUTS)
      printf("%s", (char *)x0);
    else if (svc == EL0_SYNC_EXEC)
      exec((char *)x0, (char **)x1);
    else if (svc == EL0_SYNC_FORK)
      result = fork();
    else if (svc == EL0_SYNC_PID) {
      printf("%d", getpid());
      result = getpid();
    } else if (svc == EL0_SYNC_UART_GETC)
      return uart_getc();
    else if (svc == EL0_SYNC_TEST)
      printf("count\n");
    else if (svc == EL0_SYNC_PRINT_H)
      printf("0x%08x\n", x0);
  }
  printf("");
  asm volatile("mov x1, %0" ::"r"(run_q->fp_el0));
  asm volatile("mov x2, %0" ::"r"(run_q->sp_el0));
  return result;
}

/*
 * irq entries
 */
extern void _irq_enable();
extern void _irq_disable();
void _el0_irq_entry() {
  // _irq_disable();
  if ((*(uint32_t *)CORE0_INTERRUPT_SOURCE) == 2) {
    _timer_disable();
    _timer_handler();
  }
  // _irq_enable();
}

/*
 * Virtual File System
 */
typedef struct vnode {
  struct mount *mount;
  struct vnode_operations *v_ops;
  struct file_operations *f_ops;
  void *internal;
} vnode;
struct file {
  struct vnode *vnode;
  size_t f_pos;  // The next read/write position of this opened file
  struct file_operations *f_ops;
  int flags;
};

typedef struct mount {
  struct vnode *root;
  struct filesystem *fs;
} mount;

typedef struct filesystem {
  char *name;
  int (*setup_mount)(struct filesystem *fs, struct mount *mount);
} filesystem;

typedef struct file_operations {
  int (*write)(struct file *file, void *buf, size_t len);
  int (*read)(struct file *file, void *buf, size_t len);
} file_operations;

typedef struct vnode_operations {
  int (*lookup)(struct vnode *dir_node, struct vnode **target,
                char *component_name);
  int (*create)(struct vnode *dir_node, struct vnode **target,
                char *component_name);
} vnode_operations;

mount *rootfs;
filesystem *filesystem_arr[10];

int register_filesystem(filesystem *fs) {
  /* register the file system to the kernel. */
  static int i = 0;
  filesystem_arr[i++] = fs;
  return 0;
}

int setup_mount(struct filesystem *fs, struct mount *mount) {
  mount->fs = fs;
  return 1;
}
int lookup(struct vnode *dir_node, struct vnode **target,
           char *component_name) {
  return 0;
}
int create(struct vnode *dir_node, struct vnode **target,
           char *component_name) {
  return 0;
}
void filesystem_init() {
  char *now_addr = (char *)CPIO_ARRD, *filename, *context;
  struct cpio_newc_header *cpio_header;
  mount **now = &rootfs;
  while (!strcmp("TRAILER!!!", filename = now_addr + CPIO_SIZE)) {
    mount *new = kmalloc(sizeof(mount));
    *now = new;

    vnode_operations *v_ops = kmalloc(sizeof(vnode_operations));
    v_ops->create = create;
    v_ops->lookup = lookup;

    vnode *root = kmalloc(sizeof(vnode));
    new->root = root;

    filesystem *fs = kmalloc(sizeof(filesystem));
    fs->name = filename;
    fs->setup_mount = setup_mount;
    fs->setup_mount(fs, *now);
    cpio_info(&cpio_header, &now_addr, &context);
    now = &(new->root->mount);
  }
  now = NULL;
  register_filesystem(rootfs->fs);
}
struct file *vfs_open(char *pathname, int flags) {
  /* 1. Lookup pathname from the root vnode.
   * 2. Create a new file descriptor for this vnode if found.
   * 3. Create a new file if O_CREAT is specified in flags.
   */
}
int vfs_close(struct file *file) { /* 1. release the file descriptor */
}
int vfs_write(struct file *file, void *buf, size_t len) {
  /* 1. write len byte from buf to the opened file.
   * 2. return written size or error code if an error occurs.
   */
}
int vfs_read(struct file *file, void *buf, size_t len) {
  /* 1. read min(len, readable file data size) byte to buf from the opened file.
   * 2. return read size or error code if an error occurs.
   */
}

/*
 * tmpfs
 */