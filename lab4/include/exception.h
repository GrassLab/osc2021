# define ESR_EC_SVC 0b010101

# define SVC_ISS_NOPE 0
# define SVC_ISS_GET_TIMER_VALUE 1
# define SVC_ISS_GET_TIMER_MS 2
# define SVC_ISS_PRINT_SYSTEM_TIME_ENABLE 3
# define SVC_ISS_PRINT_SYSTEM_TIME_DISABLE 4
# define SVC_ISS_SET_ONE_SHOT_TIMER  5

# define IRQ_ENABLE()    asm volatile("msr DAIFClr, 0x2");
# define IRQ_DISABLE()   asm volatile("msr DAIFSet, 0x2");


extern "C"
void general_exception_handler(
      unsigned long arg, 
      unsigned long type, 
      unsigned long esr, 
      unsigned long elr
    );


void sync_handler(
      unsigned long arg, 
      unsigned long type, 
      unsigned long esr, 
      unsigned long elr
    );

void svc_handler(
      unsigned long arg, 
      unsigned long type, 
      int iss
    );

void irq_handler();
