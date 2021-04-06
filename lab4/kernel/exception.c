#include <uart.h>
#include <printf.h>

void exception_level_info() {
  size_t r;
  asm volatile("mrs  %[result], spsr_el1": [result]"=r"(r));
  printf("spsr_el1: %x\n", r);
  asm volatile("mrs  %[result], elr_el1": [result]"=r"(r));
  printf("elr_el1: %x\n", r);
  asm volatile("mrs  %[result], esr_el1": [result]"=r"(r));
  printf("esr_el1: %x\n", r);
}