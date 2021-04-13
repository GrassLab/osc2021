## Terms


## Notes
In EL0 (least privilege), we can only access general purpose gegisters(X0~X30) and stack pointer(SP)

OS work in EL1.  (Not going to discuss EL2 and EL3)

CurrentEL is a system register, which stores the current EL level. (needs to shift right for two bits)

If a program wants to increase its EL level, participation of the software that already runs on a higher level is needed.

## process when exception occurs

    1. Address of the current instruction is saved in the ELR_ELn register. (It is called Exception link register)
    2. Current processor state is stored in SPSR_ELn register (Saved Program Status Register)
    3. An exception handler is executed and does whatever job it needs to do.
    4. Exception handler calls eret instruction. This instruction restores processor state from SPSR_ELn and resumes execution 
    5. starting from the address, stored in the ELR_ELn register.

* SPSR_ELx: store the current processor's state(PSTATE)
* ELR_ELx: save the exception return address
    * These two registers are used to ensure that we can sucessfully return to program after interrupt handler

> eret: exception return