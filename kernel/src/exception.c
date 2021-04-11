#include "exception.h"

#include "io.h"
#include "utils.h"

void exception_handler() {
  uint64_t spsr_el1, elr_el1, esr_el1;
  asm volatile("mrs %0, spsr_el1" : "=r"(spsr_el1));
  asm volatile("mrs %0, elr_el1" : "=r"(elr_el1));
  asm volatile("mrs %0, esr_el1" : "=r"(esr_el1));
  print_s("SPSR_EL1: ");
  print_h(spsr_el1);
  print_s("\n");
  print_s("ELR_EL1: ");
  print_h(elr_el1);
  print_s("\n");
  print_s("ESR_EL1: ");
  print_h(esr_el1);
  print_s("\n");
}
