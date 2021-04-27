#ifndef TRAP_H
#define TRAP_H

typedef struct saved_reg {
  unsigned long x30, pad, x28, x29, x26, x27, x24, x25, x22, x23, x20, x21;
  unsigned long x18, x19, x16, x17, x14, x15, x12, x13, x10, x11;
  unsigned long x8, x9, x6, x7, x4, x5, x2, x3, x0, x1;
} saved_reg;

#endif