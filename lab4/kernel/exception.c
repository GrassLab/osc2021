#include <uart.h>
#include <printf.h>

void exception_level_info() {
  size_t spsr_el1, elr_el1, esr_el1;
  asm volatile("mrs  %[result], spsr_el1": [result]"=r"(spsr_el1));
  printf("spsr_el1: %x\n", spsr_el1);
  asm volatile("mrs  %[result], elr_el1": [result]"=r"(elr_el1));
  printf("elr_el1: %x\n", elr_el1);
  asm volatile("mrs  %[result], esr_el1": [result]"=r"(esr_el1));
  printf("esr_el1: %x\n", esr_el1);
}