.section ".text.boot"

.globl _start

_start:
  mov  x3, x0
  mrs  x0, mpidr_el1
  and  x0, x1, #3 
  cbz  x0, _kernel_loader

_proc_hang:
  wfe
  b _proc_hang

_kernel_loader:
  ldr  x0, =_start
  mov  sp, x0
  ldr  x0, =_bss_begin
  ldr  x1, =_bss_end
_bss_init:
  str  xzr, [x0], #8
  cmp  x0, x1
  bne  _bss_init

_from_el2_to_el1:
  #set hcr_el2.RW[31] to 1 (AARCH64)
  mov  x0, #(1<<31)
  msr  hcr_el2, x0
  #set EL1 stack
  ldr  x0, =_start
  msr  sp_el1, x0
  #set exception vector table
  bl _set_exception_vector_table
  #store Pstate to spsr_el2
  mov  x0, 0x3c5 // EL1h (SPSel = 1) with interrupt disabled
  msr  spsr_el2, x0
  #return to EL1
  ldr  x0, =_from_el1_to_el0
  msr  elr_el2, x0
  eret
_from_el1_to_el0:
  #set EL0 stack
  ldr  x0, =_start
  sub  x0, x0, #0x1000
  msr  sp_el0, x0
  #store Pstate to spsr_el1
  mov  x0, #0x3c0
  msr  spsr_el1, x0
  #bl core_timer_enable
  #disable exception of access SIMD register in EL1
  mrs x0, cpacr_el1
  orr x0, x0, #(3 << 20)
  msr cpacr_el1, x0
  #return to el0
  #ldr  x0, = _load_main
  #msr  elr_el1, x0
  #eret
  bl enable_interrupt
_load_main:
  mov  x0, x3
  bl main