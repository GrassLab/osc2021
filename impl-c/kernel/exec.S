.section ".text.exc"

.global _exec_usr


# Exec code as user program
# x0: addr of the program entry point
# x1: address of user stack
# x2: process status, 0x3c0 for example
_exec_usr:
    msr elr_el1, x0
    msr sp_el0, x1
    msr spsr_el1, x2
    eret