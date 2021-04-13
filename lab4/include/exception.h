# define ESR_EC_SVC 0b010101

# define SVC_ISS_NOPE 0
# define SVC_ISS_GET_TIMER_VALUE 1
# define SVC_ISS_GET_TIMER_MS 2
# define SVC_ISS_TIMER_ENABLE 3
# define SVC_ISS_TIMER_DISABLE 4


char vector_table_desc[16][30] = {
  "0x000, Synchronous-SP_EL0",
  "0x080, IRQ-SP_EL0",
  "0x100, FIQ-SP_EL0",
  "0x180, sError-SP_EL0",
  "0x200, Synchronous-SP_ELx",
  "0x280, IRQ-SP_ELx",
  "0x300, FIQ-SP_ELx",
  "0x380, sError-SP_ELx",
  "0x400, Synchronous-AArch64",
  "0x480, IRQ-AArch64",
  "0x500, FIQ-AArch64",
  "0x580, sError-AArch64",
  "0x600, Synchronous-AArch32",
  "0x680, IRQ-AArch32",
  "0x700, FIQ-AArch32",
  "0x780, sError-AArch32",
};

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
      unsigned long esr 
    );

void svc_handler(
      unsigned long arg, 
      unsigned long type, 
      int iss
    );

void irq_handler();
