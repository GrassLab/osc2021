#define MAX_NR_BOTTOM 20
#define MAX_NR_TQE 20
#define PBASE (0x3F000000 + 0xffff000000000000)
#define IRQ_BASIC_PENDING   (PBASE+0x0000B200)
#define IRQ_PENDING_1       (PBASE+0x0000B204)
#define IRQ_PENDING_2       (PBASE+0x0000B208)
#define FIQ_CONTROL         (PBASE+0x0000B20C)
#define ENABLE_IRQS_1       (PBASE+0x0000B210)
#define ENABLE_IRQS_2       (PBASE+0x0000B214)
#define ENABLE_BASIC_IRQS   (PBASE+0x0000B218)
#define DISABLE_IRQS_1      (PBASE+0x0000B21C)
#define DISABLE_IRQS_2      (PBASE+0x0000B220)
#define DISABLE_BASIC_IRQS  (PBASE+0x0000B224)

#define SYSTEM_TIMER_IRQ_0  (1 << 0)
#define SYSTEM_TIMER_IRQ_1  (1 << 1)
#define SYSTEM_TIMER_IRQ_2  (1 << 2)
#define SYSTEM_TIMER_IRQ_3  (1 << 3)

#define CORE0_INTERRUPT_SOURCE (0x40000060 + 0xffff000000000000)

#define TICKS_FOR_SECOND 0x124F800
#define TICKS_FOR_ITR 0x124F800
#define TICKS_THRESHOLD_SECOND (TICKS_FOR_SECOND / TICKS_FOR_ITR)

#define PRIORITY_TIMER 10

#define AUX_IRQ 29

void do_core_timer_handler(void);
void first_level_irq_handler(int type);
void second_level_irq_handler(int type);
void irq_handler(void);
void btm_sched();


struct tqe {
    struct tqe *next;
    struct task *proc; // pointer to requesting process
    int tick; // expiration time count
    void *(*action)(void*);    // action function = twakeup
    void *args;
};

int timerPool_init();
int tqe_release();
struct tqe *tqe_new();
int tqe_decr(int tick);
int tqe_add(unsigned int tick, void *(*action)(void *), void *args);


struct irq_btm_task {
    int priority;   // bigger means higher priority
    int dirty;      // 1: has been runed; 0: yet
    void *(*btm_handler)(void*);
};

int irq_btm_q_adjust(int idx);
struct irq_btm_task irq_btm_q_delete_max();
int irq_btm_q_insert(int priority, void *(*btm_handler)(void*));

void do_uart_handler();
void invalid_handler();
int chk_sched();

void init_wait_pool();
struct wait_h *new_wait();
struct wait_args *new_wait_args();
void init_sleepQueue();
int add_to_waitQueue(struct task *new, struct wait_h *waitQueue);
int rm_from_queue(void* args);
