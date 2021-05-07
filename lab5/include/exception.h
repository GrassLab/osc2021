# define ESR_EC_SVC 0b010101

# define SVC_ISS_NOPE 0
# define SVC_ISS_GET_TIMER_VALUE 1
# define SVC_ISS_GET_TIMER_MS 2
# define SVC_ISS_PRINT_SYSTEM_TIME_ENABLE 3
# define SVC_ISS_PRINT_SYSTEM_TIME_DISABLE 4
# define SVC_ISS_SET_ONE_SHOT_TIMER  5
# define SVC_ISS_GETPID  6
# define SVC_ISS_UART_READ  7
# define SVC_ISS_UART_WRITE  8
# define SVC_ISS_EXEC  9
# define SVC_ISS_EXIT  10

# define IRQ_ENABLE()    asm volatile("msr DAIFClr, 0x2");
# define IRQ_DISABLE()   asm volatile("msr DAIFSet, 0x2");
# define uint64_t unsigned long long 

struct trapframe {
    uint64_t x[31]; // general register from x0 ~ x30
    uint64_t sp_el0;
    uint64_t elr_el1;
    uint64_t spsr_el1;
};

extern "C"
void general_exception_handler(
      struct trapframe *r, 
      unsigned long type, 
      unsigned long esr, 
      unsigned long elr
    );


void sync_handler(
      struct trapframe *arg, 
      unsigned long type, 
      unsigned long esr, 
      unsigned long elr
    );

void svc_handler(
      struct trapframe *arg, 
      unsigned long type, 
      int iss
    );

void irq_handler();
int get_DAIF();
