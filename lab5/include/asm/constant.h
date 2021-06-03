#ifndef __ASM_CONSTANT_H_
#define __ASM_CONSTANT_H_

#define BUILD_BUG_ON(condition) ((void)sizeof(char[1 - 2*!!(condition)]))
#define PT_REGS_SIZE 0x110

#endif