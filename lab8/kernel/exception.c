#include <uart.h>
#include <printf.h>
#include <syscall.h>

void exception_level_info() {
  int32_t spsr_el1, esr_el1;
  size_t elr_el1, sp_el0, sp;
  asm volatile("mrs  %0, spsr_el1": "=r"(spsr_el1));
  printf("spsr_el1: %x\n", spsr_el1);
  asm volatile("mrs  %0, elr_el1\n": "=r"(elr_el1));
  printf("elr_el1: %x\n", elr_el1);
  asm volatile("mrs  %0, esr_el1\n": "=r"(esr_el1));
  printf("esr_el1: %x\n", esr_el1);
  asm volatile("mrs  %0, sp_el0\n": "=r"(sp_el0));
  printf("sp_el0: %x\n", sp_el0);
  asm volatile("mov  %0, sp\n": "=r"(sp));
  printf("sp: %x\n", sp);
  while(1) {

  } 
}

void syscall_number_error() {
  printf("syscall_number_error\n");
}

void page_fault_handler() {
  size_t far_el1;
  asm volatile("mrs  %0, far_el1": "=r"(far_el1));
  printf("far_el1: %x\n", far_el1);

  do_exit(-1);
}